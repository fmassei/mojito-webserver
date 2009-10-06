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

/* FIXME change that thing */
struct module_s *ch_filter;

/* extract url and query string from the uri */
static void strip_uri(struct request_s *req)
{
    int p;
    /* divide by host and query_string */
    if ((req->qs = index(req->uri, '?'))==NULL) { /* no qs */
        req->page = req->uri;
    } else {
        /* strip the '?' character */
        ++req->qs; 
        /* copy and terminate the 'page' string */
        p = (int)(req->qs - req->uri - 1);
        req->page = malloc(p+1);
        memcpy(req->page, req->uri, p);
        req->page[p] = '\0';
    }
}

static int find_file(struct request_s *req)
{
    extern struct fparam_s params;
    req->abs_filename = malloc(strlen(params.http_root)+strlen(req->page)+1);
    if (req->abs_filename==NULL) {
        return HRESP_500;
    }    
    sprintf(req->abs_filename, "%s%s", params.http_root, req->page);
redo:
    memset(&req->sb, 0, sizeof(req->sb));
    stat(req->abs_filename, &req->sb);
    if ((req->sb.st_mode&S_IFMT)!=S_IFREG) {
        if ((req->sb.st_mode&S_IFMT)==S_IFDIR) {
            if ((req->abs_filename = realloc(req->abs_filename,
                                        strlen(req->abs_filename)+
                                        strlen(params.default_page)+1))==NULL) {
                return HRESP_500;
            }
            sprintf(req->abs_filename, "%s%s", req->abs_filename,
                                                        params.default_page);
            goto redo;
        }
        return HRESP_404;
    }
    return 0;
}

/* check if we can deal the request header */
static int check_method(struct request_s *req)
{
    if (req->method!=M_GET && req->method!=M_POST && req->method!=M_HEAD) {
        return -1;
    }
    return 0;
}

/* send the response */
void send_file(int sock, struct request_s *req)
{
    extern struct fparam_s params;
    extern int keeping_alive, content_length_sent;
    unsigned char *addr;
    int fd, find_ret;

    if (check_method(req)!=0) {
        header_kill_w_code(HRESP_501, sock);
        return;
    }
    strip_uri(req);
    find_ret = find_file(req);
    /* find_ret can be an error, but maybe we're dealing with a "special" file.
     * We run can_run() and on_presend() to see if there is at least one module
     * that can process the request */
    if (can_run(req)!=0)
        return;
    if (on_presend(sock, req)!=0)
        return;
    if (find_ret!=0) {
        header_kill_w_code(find_ret, sock);
        return;
    }
    if ((ch_filter = filter_findfilter(req->header.accept_encoding))==NULL) {
        header_kill_w_code(HRESP_406, sock);
        return;
    }
    if ((fd = open(req->abs_filename, O_RDONLY))<=0) {
        header_kill_w_code(HRESP_404, sock);
        return;
    }
    header_push_code(HRESP_200);
    header_push_contenttype(mime_gettype(req->abs_filename));
    if (on_prehead(&req->sb)!=0)
        return;
    /* no length? no party. We can't keep the connection alive 
     * FIXME this is very ugly. Move this check somewhere else */
    if (keeping_alive==1 && content_length_sent==0)
        keeping_alive = 0;
    header_send(sock);
    if (req->method==M_HEAD)
        return;
    addr = mmap(NULL, req->sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (on_send(addr, sock, &req->sb)!=0)
        return;
    if (on_postsend(req, mime_gettype(req->abs_filename), addr, &req->sb)!=0)
        return;
}

