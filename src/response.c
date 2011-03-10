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
#include <mmp/mmp_mmap.h>
#include "../common_src/resp_headers.h"

int hresp2int(t_hresp_e resp)
{
    switch(resp) {
    case HRESP_200: return 200;
    case HRESP_404: return 404;
    case HRESP_406: return 406;
    case HRESP_500: return 500;
    case HRESP_501: return 501;
    }
    return 0;
}

void response_init(t_response_s *res, int min_keep)
{
    *res->resbuf = *res->tmpbuf = '\0';
    if (!min_keep)
        res->sock = SOCKET_INVALID;
    res->ch_filter = NULL;
    res->content_length_sent = 0;
    res->rstate.fd = -1;
    res->rstate.sent = 0;
    res->rstate.mod_res.data = NULL;
    res->final_data_sent = 0;
    module_fill_response_vector(res);
}

void response_drop(t_response_s *resp, int min_keep)
{
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

static t_hresp_e find_file(t_request_s *req, t_response_s *res)
{
    const t_config_s *config;
    config = config_get();
    req->abs_filename = xmalloc(strlen(config->server->http_root)+strlen(req->page)+1);
    if (req->abs_filename==NULL) {
        return HRESP_500;
    }    
    sprintf(req->abs_filename, "%s%s", config->server->http_root, req->page);
redo:
    memset(&res->rstate.sb, 0, sizeof(res->rstate.sb));
    mmp_stat(req->abs_filename, &res->rstate.sb);
    if ((res->rstate.sb.st_mode&S_IFMT)!=S_IFREG) {
        if ((res->rstate.sb.st_mode&S_IFMT)==S_IFDIR) {
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
    req->mime_type = mime_gettype(req->abs_filename);
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
t_response_send_e response_send(t_socket_unit_s *su)
{
    int find_ret;
    t_modret_e mod_ret;
    t_request_s *req = &su->req;
    t_response_s *res = &su->res;

    if (res->rstate.fd>=0)
        goto just_continue;

    res->sock = su->socket;

    if (check_method(req)!=0) {
        header_kill_w_code(res, HRESP_501, req->protocol);
        return RESPONSE_SEND_FINISH;
    }
    strip_uri(req);
    find_ret = find_file(req, res);
    /* find_ret can be an error, but maybe we're dealing with a "special" file.
     * We run can_run() and on_presend() to see if there is at least one module
     * that can process the request */
    if (can_run(req, res)!=MODRET_OK)
        return RESPONSE_SEND_FINISH;
    mod_ret = on_presend(req, res);
    if (mod_ret==MODRET_ALLDONE)
        return RESPONSE_SEND_MODDONE;
    else if (mod_ret!=MODRET_OK)
        return RESPONSE_SEND_FINISH;
    if (find_ret!=0) {
        header_kill_w_code(res, find_ret, req->protocol);
        return RESPONSE_SEND_FINISH;
    }
    if ((res->ch_filter = filter_findfilter(req->accept_encoding))==NULL) {
        header_kill_w_code(res, HRESP_406, req->protocol);
        return RESPONSE_SEND_FINISH;
    }
    header_push_code(res, HRESP_200, req->protocol);
    header_push_contenttype(res, req->mime_type);
    res->ch_filter->on_prehead(res);
    if ((mod_ret = on_prehead(res))!=MODRET_OK) {
        return RESPONSE_SEND_FINISH;
    }
    /* no length? no party. We can't keep the connection alive 
     * FIXME this is very ugly. Move this check somewhere else */
    if (req->keeping_alive==1 && res->content_length_sent==0)
        req->keeping_alive = 0;
    header_send(res);
    if (req->method==REQUEST_METHOD_HEAD) {
        return RESPONSE_SEND_FINISH;
    }
    if ((res->rstate.fd = mmp_open(req->abs_filename, O_RDONLY, 0))<=0) {
        header_kill_w_code(res, HRESP_404, req->protocol);
        return RESPONSE_SEND_FINISH;
    }
just_continue:
    mod_ret = filter_on_send(res->ch_filter, res);
    if (mod_ret==MODRET_CONTINUE)
        return RESPONSE_SEND_CONTINUE;
    if (mod_ret==MODRET_ERR) {
        mmp_trace_print(stdout);
        mmp_close(res->rstate.fd);
        return RESPONSE_SEND_ERROR;
    }
    if (on_postsend(req, res)!=MODRET_OK) {
        mmp_close(res->rstate.fd);
        return RESPONSE_SEND_ERROR;
    }
    mmp_close(res->rstate.fd);
    return RESPONSE_SEND_FINISH;
}
