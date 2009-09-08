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

#include "response.h"

/* final filter(s) */
static struct module_filter_s *filter;

static char *page;
char *query_string;

/* extract url and query string from the uri */
static void strip_uri(const char *uri)
{
    int p;
    /* divide by host and query_string */
    if ((query_string = index(uri, '?'))==NULL) { /* no qs */
        page = (char*)uri;
    } else {
        /* strip the '?' character */
        ++query_string; 
        /* copy and terminate the 'page' string */
        p = (int)(query_string - uri - 1);
        page = malloc(p+1);
        memcpy(page, uri, p);
        page[p] = '\0';
    }
}

/* check if we can deal the request header */
static int check_method()
{
    extern struct request_s req;
    if (req.method!=M_GET && req.method!=M_POST && req.method!=M_HEAD) {
        return -1;
    }
    return 0;
}

/* send the response */
void send_file(int sock, const char *uri)
{
    extern fparams_st params;
    extern struct request_s req;
    extern int keeping_alive;
    struct stat sb;
    unsigned char *addr;
    char *filename;
    long clen;
    int fd;

    if (check_method()!=0) {
        header_kill_w_code(HRESP_501, sock);
        return;
    }
    if (on_presend(sock, &req)!=0)
        return;
    /* no cache? build one */
    strip_uri(uri);
    if ((filename = malloc(strlen(params.http_root)+strlen(page)+1))==NULL) {
        header_kill_w_code(HRESP_500, sock);
        return;
    }    
    sprintf(filename, "%s%s", params.http_root, page);
redo:
    memset(&sb, 0, sizeof(sb));
    stat(filename, &sb);
    if ((sb.st_mode&S_IFMT)!=S_IFREG) {
        if ((sb.st_mode&S_IFMT)==S_IFDIR) {
            if ((filename = realloc(filename, strlen(filename)+
                                        strlen(params.default_page)+1))==NULL) {
                header_kill_w_code(HRESP_500, sock);
                return;
            }
            sprintf(filename, "%s%s", filename, params.default_page);
            goto redo;
        }
        header_kill_w_code(HRESP_404, sock);
        return;
    }
    if (access(filename, X_OK)==0) {
        cgi_run(filename, sock);
        /* if here the cgi failed. Send a 500 to the client */
        header_kill_w_code(HRESP_500, sock);
        return;
    }
    filter = filter_findfilter(req.header.accept_encoding);
    if (filter==NULL) {
        header_kill_w_code(HRESP_406, sock);
        return;
    }
    if ((fd = open(filename, O_RDONLY))<=0) {
        header_kill_w_code(HRESP_404, sock);
        return;
    }
    header_push_code(HRESP_200);
    if ((clen = filter->prelen(&sb))>=0) {
        header_push_contentlength(clen);
    } else {
        /* no length? no party. We can't keep the connection alive */
        keeping_alive = 0;
    }
    header_push_contentencoding(filter->name);
    header_push_contenttype(mime_gettype(filename));
    header_send(sock);
    if (req.method==M_HEAD)
        return;
    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    filter->compress(addr, sock, sb.st_size);
    /* add in cache (only if filter != identity!) */
    if (!strcmp(filter->name, "identity"))
        return;
    on_postsend(&req, filter, mime_gettype(filename), addr, sb.st_size);
}

