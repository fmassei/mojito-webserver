/*
    Copyright 2009 Francesco Massei

    This file is part of mojito webserver.

        Mojito is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Mojito is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Mojito.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "request.h"

char *method_strs[] = { "", "GET" /* M_GET */, "HEAD", "POST" };

char *uri;
char *in_ip;
char *method_str;
struct qhead_s *accept_encoding = NULL;
char *post_data = NULL;
int post_fd = 0;
int method = 0;
char *content_type = NULL;
long content_length = 0;
int proto_version = P_HTTP_UNK;

static char *str_content_length=NULL;
static int status;
static char *buf, *cur_head;

/* create an empty request */
void request_create()
{
    uri = NULL;
    method = 0;
    status = HEAD;
    buf = cur_head = NULL;
}

/* wait on a socket for a specific timeout */
int request_waitonalive(int sock)
{
    extern struct fparams_s params;
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);
    tv.tv_sec = params.keepalive_timeout;
    tv.tv_usec = 0;
    return select(sock+1, &rfds, NULL, NULL, &tv);
}

/* parse the request first line */
static int parse_first_line(char *line)
{
    extern int keeping_alive;
    char *st, *st2;
    if (!memcmp(line, "GET ", 4))
        method = M_GET;
    else if (!memcmp(line, "HEAD ", 5))
        method = M_HEAD;
    else if (!memcmp(line, "POST ", 5))
        method = M_POST;
    else 
        return -1;
    method_str = method_strs[method];
    /* go to next arg. Here and below we skip more than one whitespace if
     * present between arguments. This is not RFC2616 conforming, but adds
     * some robustness to the client/server communication. */
    st = line + 3;
    if (*st!=' ') ++st;
    while(*(++st)==' ' && *st!='\0');
    /* find end or space */
    for (st2=st; *st2!='\0' && *st2!=' '; ++st2);
    uri = malloc(st2-st+1);
    memcpy(uri, st, st2-st);
    uri[st2-st] = '\0';
    /* find protocol version */
    st = st2;
    while(*(++st)==' ' && *st!='\0');
    /* again, we skip whitespaces at the end, breaking the specs. */
    for (st2=st; *st2!='\0' && *st2!=' '; ++st2);
    proto_version = P_HTTP_UNK;
    if ((st2-st)>=8) {
        if (!memcmp(st, "HTTP/1.1", 8)) {
            proto_version = P_HTTP_11;
            keeping_alive = 1;  /* by default (RFC2616-8.1.2) */
        }
        else if (!memcmp(st, "HTTP/1.0", 8))
            proto_version = P_HTTP_10;
    }
    return 0;
}

/* parse a generic header line */
static int parse_header_line(char *line)
{
    extern int keeping_alive;
    char *value, c;
    char *endptr; /* useded by strtol */
    if ((value = strchr(line, ':'))==NULL)
        return 0;
    *value++ = '\0';
    while ((c=*value)&&(c==' ' || c=='\t'))
        ++value;
    to_upper(line);
    if (!memcmp(line, "CONTENT_LENGTH", 15) && !str_content_length) {
        content_length = strtol(value, &endptr, 10);
        /* try to convert content_length in a valid number */
        if ((errno==ERANGE
                && (content_length==LONG_MAX || content_length==LONG_MIN))
                || (errno!=0 && value==0) || (endptr==value)) {
            content_length = 0;
            return 0;
        }
        str_content_length = value;
    } else if (!memcmp(line, "ACCEPT_ENCODING", 15) && !accept_encoding) {
        DEBUG_LOG((LOG_DEBUG, "parsing accepted encoding: %s", value));
        accept_encoding = qhead_parse(value);
    } else if (!memcmp(line, "CONTENT_TYPE", 12) && !content_type) {
        content_type = value;
    } else if (!memcmp(line, "CONNECTION", 10)) {
        if ( (proto_version==P_HTTP_10) &&
                (!strncasecmp(value, "keep-alive", strlen(value))) )
            keeping_alive = 1;
        else if ( (proto_version==P_HTTP_11) &&
                (!strncasecmp(value, "close", strlen(value))) )
            keeping_alive = 0;
    }
    return 0;
}

/* parse an option */
static int parse_option(char *line)
{
    if (method==0)
        return parse_first_line(line);
    else
        return parse_header_line(line);
}

/* ok - let's the kernel do its job :-) For post-data we create a temporary 
 * file to pass to cgi apps. We rely on two very important features: buffering
 * and delayed unlink. As long as I know every unix-like kernel behave the
 * same way in our conditions. */
static int create_post_file()
{
    extern fparams_st params;
    static char tmp_file[2049];
    int fd;
    snprintf(tmp_file, 2049, "%s/mojito-temp.XXXXXX", params.tmp_dir);
    if ((fd = mkstemp(tmp_file))==-1) {
        perror("mkstemp");
        return -1;
    }
    if (unlink(tmp_file)==-1) {
        close(fd);
        perror("unlink");
        return 0; /* FIXME: Right now we go on without post-data. Maybe it's not
                    * the safer choice. Think about just returning 501. */
    }
    return fd;
}

/* cycle the socket data, reading SOCKBUFSIZE bytes a time, until we got
 * everything */
int request_read(int sock)
{
    int reline;
    int rb, mr, len, pos;
    char c;
    len = pos = 0;
    while(1) {
        mr = SOCKBUFSIZE + pos;
        if (len<mr) {
            reline = (int)cur_head - (int)buf;
            buf = realloc(buf, mr);
            cur_head = (char*)((int)buf + reline);
            len = mr;
        }
        rb = read(sock, buf+pos, SOCKBUFSIZE);
        if (rb<=0)
            break;
        /* BOA-like loop. An insane mess, but pretty clear after two or three
         * hours spent on reading this twenty lines. Good luck. */
        while(rb-->0) {
            c = *(buf+pos);
            if (c=='\0') {
                continue;
            }
            switch(status) {
            case HEAD:
                if (c=='\r')
                    status = CR1;
                else if (c=='\n')
                    status = LF1;
                break;
            case CR1:
                if (c=='\n')
                    status = LF1;
                else if (c=='\r')
                    status = HEAD;
                break;
            case LF1:
                if (c=='\r')
                    status = CR2;
                else if (c=='\n')
                    status = BODY;
                else
                    status = HEAD;
                break;
            case CR2:
                if (c=='\n')
                    status = BODY;
                else if (c=='\r')
                    status = HEAD;
                break;
            }
            ++pos;
            if (status==LF1) {
                /* header found */
                buf[pos-2] = '\0';
                parse_option(cur_head);
                cur_head = buf+pos;
            } else if (status==BODY) {
                /* sanitize accept_encoding. */
                filter_sanitize_queue(&accept_encoding);
                if (content_length<=0 || method!=M_POST)
                    return 0;
                /* I was tempted to drop the headers freeing some memory. I
                 * don't do this here because maybe we'll need them in a
                 * (hopefully near) future, in a FastCGI implementation. */
                post_data = buf+pos;
                if (content_length > rb) {
                    /* if here we have to realloc the buffer and get the
                     * missing data stored in the socket buffer */
                    buf = realloc(buf, mr-rb+content_length);
                    do {
                        len = content_length - rb;                
                        rb = read(sock, buf+pos, len);
                        len -= rb;
                        pos += rb;
                    } while(len>0);
                }
                /* write to file */
                if ((post_fd = create_post_file())==-1)
                    return -1;
                write(post_fd, post_data, content_length);
/*#ifndef NDEBUG
{
    int q;
    printf("POSTDATA: ");
    for (q=0; q<content_length; ++q)
        printf("%c", *(post_data+q));
    printf("\n");
}
#endif*/
                return 0;
            }
        }
    }
    return 1;
}

