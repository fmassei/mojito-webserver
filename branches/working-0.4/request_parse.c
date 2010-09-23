/*
    Copyright 2010 Francesco Massei

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
#include "request_parse.h"

#define SOCKBUFSIZE 8196    /* TODO: calculate this one! */

/* parse the request first line */
static int parse_first_line(t_request_s *req, char *line)
{
    char *st, *st2;
    if (memcmp(line, "GET ", 4))
        req->method = REQUEST_METHOD_GET;
    else if (memcmp(line, "HEAD ", 5))
        req->method = REQUEST_METHOD_HEAD;
    else if (memcmp(line, "POST ", 5))
        req->method = REQUEST_METHOD_POST;
    else
        return -1;
    /* go to next arg. Here and below we skip more than one whitespace if
     * present between arguments. This is not RFC2616 conforming, but adds
     * some robustness to the client/server communication. */
    st = line + 3;
    if (*st!=' ') ++st;
    while (*(++st)==' ' && *st!='\0') ;
    /* find end or space */
    for (st2=st; *st2!='\0' && *st2!=' '; ++st2);
    req->URI = xmalloc(st2-st+1);
    memcpy(req->URI, st, st2-st);
    req->URI[st2-st] = '\0';
    /* find protocol version */
    st = st2;
    while(*(++st)==' ' && *st!='\0');
    /* again, we skip whitespaces at the end, breaking the specs. */
    for (st2=st; *st2!='\0' && *st2!=' '; ++st2);
    /* get protocol */
    req->protocol = REQUEST_PROTOCOL_UNKNOWN;
    if ((st2-st)>=8) {
        if (!memcmp(st, "HTTP/1.1", 8)) {
            req->protocol = REQUEST_PROTOCOL_HTTP11;
            req->keeping_alive = 1;  /* by default (RFC2616-8.1.2) */
        }
        else if (!memcmp(st, "HTTP/1.0", 8))
            req->protocol = REQUEST_PROTOCOL_HTTP10;
    }
    return 0;
}

/* a smarter toupper() function */
static char *head_to_upper(char *str)
{
    char *st = str;
    while (*str) {
        if (*str=='-') *str = '_';
        else *str = (char)toupper(*str);
        ++str;
    }
    return st;
}

/* parse a generic header line */
static int parse_header_line(t_request_s *req, char *line)
{
    char *value, c;
    char *endptr; /* useded by strtol */
    if ((value = strchr(line, ':'))==NULL)
        return 0;
    *value++ = '\0';
    for(c=*value; c!='\0' || c==' ' || c=='\t'; c=*value)
        ++value;
    head_to_upper(line);
    if (!memcmp(line, "CONTENT_LENGTH", 15) && req->content_length==0) {
        req->content_length = strtol(value, &endptr, 10);
        /* try to convert content_length in a valid number */
        if ((errno==ERANGE
                && (req->content_length==LONG_MAX || 
                    req->content_length==LONG_MIN))
                || (errno!=0 && value==0) || (endptr==value)) {
            req->content_length = 0;
            return 0;
        }
    } else if (!memcmp(line, "ACCEPT_ENCODING", 15) &&
            !req->accept_encoding) {
        req->accept_encoding = qhead_list_parse(value);
    } else if (!memcmp(line, "CONTENT_TYPE", 12) && req->content_type==NULL) {
        req->content_type = value;
    } else if (!memcmp(line, "CONNECTION", 10)) {
        if ( (req->protocol==REQUEST_PROTOCOL_HTTP10) &&
                (!xstrncasecmp(value, "keep-alive", strlen(value))) )
            req->keeping_alive = 1;
        else if ( (req->protocol==REQUEST_PROTOCOL_HTTP11) &&
                (!xstrncasecmp(value, "close", strlen(value))) )
            req->keeping_alive = 0;
    }
    return 0;
}

/* parse an option */
static int parse_option(t_request_s *req, char *line)
{
    if (req->method==0)
        return parse_first_line(req, line);
    else
        return parse_header_line(req, line);
}

t_request_parse_e request_parse_read(t_socket *sock, t_request_s *req)
{
    req->parse.len = req->parse.pos = 0;
#ifdef _WIN32
#   pragma warning(push)
#   pragma warning(disable:4127)
#endif
    while(1) {
#ifdef _WIN32
#   pragma warning(pop)
#endif
        req->parse.mr = SOCKBUFSIZE + req->parse.pos;
        if (req->parse.len<req->parse.mr) {
            req->parse.reline =(int)(req->parse.cur_head)-(int)(req->parse.buf);
            req->parse.buf = xrealloc(req->parse.buf, req->parse.mr);
            req->parse.cur_head =(char*)((int)req->parse.buf+req->parse.reline);
            req->parse.len = req->parse.mr;
        }
        req->parse.rb =
            mmp_socket_read(sock, req->parse.buf+req->parse.pos, SOCKBUFSIZE);
        if (req->parse.rb==SOCKET_ERROR) {
            if (mmp_socket_is_block_last_error())
                return REQUEST_PARSE_CONTINUE;
            return REQUEST_PARSE_ERROR;
        }
        if (req->parse.rb==0) {
            return REQUEST_PARSE_CLOSECONN;
        }
        /* BOA-like loop. An insane mess, but pretty clear after two or three
         * hours spent on reading this twenty lines. Good luck. */
        while(req->parse.rb-->0) {
            req->parse.c = *(req->parse.buf+req->parse.pos);
            if (req->parse.c=='\0')
                continue;
            switch(req->parse.status) {
            case REQUEST_PARSE_STATUS_HEAD:
                if (req->parse.c=='\r')
                    req->parse.status = REQUEST_PARSE_STATUS_CR1;
                else if (req->parse.c=='\n')
                    req->parse.status = REQUEST_PARSE_STATUS_LF1;
                break;
            case REQUEST_PARSE_STATUS_CR1:
                if (req->parse.c=='\n')
                    req->parse.status = REQUEST_PARSE_STATUS_LF1;
                else if (req->parse.c=='\r')
                    req->parse.status = REQUEST_PARSE_STATUS_HEAD;
                break;
            case REQUEST_PARSE_STATUS_LF1:
                if (req->parse.c=='\r')
                    req->parse.status = REQUEST_PARSE_STATUS_CR2;
                else if (req->parse.c=='\n')
                    req->parse.status = REQUEST_PARSE_STATUS_BODY;
                else
                    req->parse.status = REQUEST_PARSE_STATUS_HEAD;
                break;
            case REQUEST_PARSE_STATUS_CR2:
                if (req->parse.c=='\n')
                    req->parse.status = REQUEST_PARSE_STATUS_BODY;
                else if (req->parse.c=='\r')
                    req->parse.status = REQUEST_PARSE_STATUS_HEAD;
                break;
            default:
                /* nothing */
                break;
            }
            ++req->parse.pos;
            if (req->parse.status==REQUEST_PARSE_STATUS_LF1) {
                /* header found */
                req->parse.buf[req->parse.pos-2] = '\0';
                parse_option(req, req->parse.cur_head);
                req->parse.cur_head = req->parse.buf+req->parse.pos;
            } else if (req->parse.status==REQUEST_PARSE_STATUS_BODY) {
                filter_sanitize_queue(&req->accept_encoding);
                if (req->content_length<=0 && req->method!=REQUEST_METHOD_POST)
                    return REQUEST_PARSE_FINISH;
            }
        }
    }
    return REQUEST_PARSE_ERROR; /* this should never happen */
}

