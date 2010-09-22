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

t_response_s *response_create(void)
{
    t_response_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    ret->resbuf = ret->tmpbuf = NULL;
    ret->sock = SOCKET_INVALID;
    ret->ch_filter = NULL;
    ret->content_length_sent = 0;
    return ret;
}

void response_destroy(t_response_s **resp)
{
    if ((*resp)->resbuf!=NULL)
        xfree((*resp)->resbuf);
    if ((*resp)->tmpbuf!=NULL)
        xfree((*resp)->tmpbuf);
    xfree(*resp);
    *resp = NULL;
}

/* extract url and query string from the uri */
static void strip_uri(t_request_s *req)
{
    int p;
    /* divide by host and query_string */
    if ((req->query_string = xindex(req->URI, '?'))==NULL) {
        /* no query_string */
        req->page = req->URI;
    } else {
        /* strip the '?' character */
        ++req->query_string; 
        /* copy and terminate the 'page' string */
        p = (int)(req->query_string - req->URI - 1);
        req->page = xmalloc(p+1);
        memcpy(req->page, req->URI, p);
        req->page[p] = '\0';
    }
}

static t_hresp_e find_file(t_request_s *req)
{
    const t_config_s *config;
    config = config_get();
    req->abs_filename = xmalloc(strlen(config->server->http_root)+strlen(req->page)+1);
    if (req->abs_filename==NULL) {
        return HRESP_500;
    }    
    sprintf(req->abs_filename, "%s%s", config->server->http_root, req->page);
redo:
    memset(&req->file_stat, 0, sizeof(req->file_stat));
    mmp_stat(req->abs_filename, &req->file_stat);
    if ((req->file_stat.st_mode&S_IFMT)!=S_IFREG) {
        if ((req->file_stat.st_mode&S_IFMT)==S_IFDIR) {
            if ((req->abs_filename = xrealloc(req->abs_filename,
                                        strlen(req->abs_filename)+
                                        strlen(config->server->default_page)+1))==NULL) {
                return HRESP_500;
            }
            sprintf(req->abs_filename, "%s%s", req->abs_filename,
                                                config->server->default_page);
            goto redo;
        }
        return HRESP_404;
    }
    return HRESP_200;
}

/* check if we can deal the request header */
static int check_method(t_request_s *req)
{
    if (req->method!=REQUEST_METHOD_GET && req->method!=REQUEST_METHOD_POST &&
                                        req->method!=REQUEST_METHOD_HEAD) {
        return -1;
    }
    return 0;
}

/* send the response */
void response_send(t_response_s *res, t_request_s *req)
{
    const t_config_s *config;
    t_mmp_mmap_s *filemap;
    int fd, find_ret;

    config = config_get();
    if (check_method(req)!=0) {
        header_kill_w_code(res, HRESP_501, req->protocol);
        return;
    }
    strip_uri(req);
    find_ret = find_file(req);
    /* find_ret can be an error, but maybe we're dealing with a "special" file.
     * We run can_run() and on_presend() to see if there is at least one module
     * that can process the request */
    if (can_run(req)!=0)
        return;
    if (on_presend(res->sock, req)!=0)
        return;
    if (find_ret!=0) {
        header_kill_w_code(res, find_ret, req->protocol);
        return;
    }
    if ((res->ch_filter = filter_findfilter(req->accept_encoding))==NULL) {
        header_kill_w_code(res, HRESP_406, req->protocol);
        return;
    }
    if ((fd = mmp_open(req->abs_filename, O_RDONLY, 0))<=0) {
        header_kill_w_code(res, HRESP_404, req->protocol);
        return;
    }
    header_push_code(res, HRESP_200, req->protocol);
    header_push_contenttype(res, mime_gettype(req->abs_filename));
    if (on_prehead(&req->file_stat, res)!=0)
        return;
    /* no length? no party. We can't keep the connection alive 
     * FIXME this is very ugly. Move this check somewhere else */
    if (req->keeping_alive==1 && res->content_length_sent==0)
        req->keeping_alive = 0;
    header_send(res);
    if (req->method==REQUEST_METHOD_HEAD)
        return;
    filemap = mmp_mmap(NULL, req->file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (on_send(filemap->ptr, &req->file_stat, res)!=0) {
        mmp_munmap(&filemap);
        return;
    }
    if (on_postsend(req, mime_gettype(req->abs_filename), filemap->ptr, &req->file_stat)!=0) {
        mmp_munmap(&filemap);
        return;
    }
}
