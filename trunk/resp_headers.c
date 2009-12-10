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
#include "resp_headers.h"

/* TODO FIXME super ugly */
int content_length_sent = 0;

/* protocol strings */
static const char HTTP10[] = "HTTP/1.0 ";
static const char HTTP11[] = "HTTP/1.1 ";
/* some status responses */
static const char *RESP[] = {
        "200 OK\r\n",
        "404 Not Found\r\n",
        "406 Not Acceptable\r\n",
        "500 Internal Server Error\r\n",
        "501 Not Implemented\r\n"
    };

static char buf[0xff];
static char res[0xff];

static void header_send_hs(char *h, char *s)
{
    sprintf(buf, "%s: %s\r\n", h, s);
    strcat(res, buf);
}

static void header_send_hl(char *h, long l)
{
    sprintf(buf, "%s: %lu\r\n", h, l);
    strcat(res, buf);
}

void header_push_code(int code)
{
    extern struct fparam_s params;
    extern int proto_version;
    res[0] = '\0';
    /* RFC2145 - conservative approach */
    if (proto_version==P_HTTP_11)
        strcat(res, HTTP11);
    else strcat(res, HTTP10);
    strcat(res, RESP[code]);
    header_send_hs("Date", time_1123_format(time(NULL)));
    header_send_hs("Server", params.server_meta);
}

void header_push_contentlength(long len)
{
    header_send_hl("Content-Length", len);
    content_length_sent = 1;
}

void header_push_contenttype(char *name)
{
    header_send_hs("Content-Type", name);
}

void header_push_contentencoding(char *name)
{
    if (!strcmp(name, "identity"))
        return;
    header_send_hs("Content-Encoding", name);
}

void header_part_send(int sock)
{
    write(sock, res, strlen(res));
}

void header_send(int sock)
{
    strcat(res, "\r\n");
    write(sock, res, strlen(res));
}

static int get_error_page(int code, size_t *len, char **fname)
{
    extern struct fparam_s params;
    struct stat sb;
    char codenum[4];
    int i;
    if (params.http_errors_root==NULL)
        return -1;
    if ((*fname = malloc(strlen(params.http_errors_root)+1+8+1))==NULL)
        return -1;
    /* FIXME: check for this one below */
    for (i=0; i<3; ++i) codenum[i] = RESP[code][i];
    codenum[3] = '\0';
    sprintf(*fname, "%s/%s.html", params.http_errors_root, codenum);
    DEBUG_LOG((LOG_DEBUG, "Trying to get error page %s", *fname));
    memset(&sb, 0, sizeof(sb));
    stat(*fname, &sb);
    if ((sb.st_mode&S_IFMT)!=S_IFREG) {
        free(*fname);
        return -1;
    }
    *len = sb.st_size;
    return 0;
}

void header_kill_w_code(int code, int sock)
{
    size_t len;
    char *fname;
    int fd = -1;
    void* addr = NULL;
    if ((get_error_page(code, &len, &fname)<0) ||
            ((fd = open(fname, O_RDONLY))<0) ||
            ((addr = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0))==NULL)) {
        header_push_code(code);
        header_push_contentlength(0);
        header_send(sock);
        if (addr) munmap(addr, len);
        if (fd>=0) close(fd);
    } else {
        header_push_code(code);
        header_push_contentlength(len);
        header_send(sock);
        on_send(addr, sock, len);
    }
}

