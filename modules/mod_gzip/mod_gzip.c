/*
    Copyright 2011 Francesco Massei

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

#include <stdio.h>
#include <stdlib.h>
#include <mmp/mmp_memory.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_socket.h>
#include <mmp/mmp_list.h>
#include "../common_src/resp_headers.h"
#include "../src/modules.h"
#include <zlib.h>

#define DEFAULT_CHUNK_LEN (16*1024)

static size_t s_chunk_length = DEFAULT_CHUNK_LEN;
static t_mmp_list_s *s_use_on_mimes;

/* TODO: missing parameter checks! */

#ifdef _WIN32
#   define OUTLINK  __declspec(dllexport)
#else
#   define OUTLINK
#endif

static int _prelen(t_mmp_stat_s *sb)
{
    if (sb->st_size==0)
        return 0;
    return -1;
}

static int is_mime_processable(const char *mime)
{
    t_mmp_listelem_s *el;
    char *p;
    for (el=s_use_on_mimes->head; el!=NULL; el=el->next) {
        p = (char*)el->data;
        if (p==NULL) continue;
        if (!strcmp(p, mime))
            return 1;
    }
    return 0;
}

static t_module_ret_e _init(void)
{
    if ((s_use_on_mimes = mmp_list_create())==NULL)
        return MOD_ERR;
    return MOD_OK;
}

static void freestr(char **str) {
    if (str==NULL || *str==NULL) return;
    MMP_XFREE_AND_NULL(*str);
}
static void freestr_v(void **ptr) { freestr((char**)ptr); }
static t_module_ret_e _fini(void)
{
    if (s_use_on_mimes!=NULL)
        mmp_list_delete_withdata(&s_use_on_mimes, freestr_v);
    return MOD_OK;
}

static t_module_ret_e _set_params(t_config_module_s *cfg_mod)
{
    size_t tmpl;
    t_mmp_listelem_s *el;
    t_config_module_setting_s *set;
    MMP_CHECK_OR_RETURN((cfg_mod!=NULL && cfg_mod->settings!=NULL), MOD_OK);
    for (el=cfg_mod->settings->head; el!=NULL; el=el->next) {
        set = (t_config_module_setting_s*)el->data;
        if (!strcmp(set->key, "chunk_length")) {
            char *endptr;
            tmpl = strtol(set->val, &endptr, 10);
            if ((errno==ERANGE
                        && (tmpl==LONG_MAX || tmpl==LONG_MIN))
                    || (errno!=0 && tmpl==0) || (endptr==set->val))
                printf("Invalid setting %s = %s\n", set->key, set->val);
            else
                s_chunk_length = tmpl;
            printf("chunk_length set to %d\n", s_chunk_length);
        } else if (!strcmp(set->key, "use_on_mime")) {
            char *copyval;
            if (set->val==NULL) continue;
            if ((copyval = xstrdup(set->val))==NULL) {
                printf("could not set mime %s\n", set->val);
                continue;
            }
            if (mmp_list_add_data(s_use_on_mimes, copyval)!=MMP_ERR_OK) {
                printf("could not set mime %s\n", set->val);
                xfree(copyval);
                continue;
            }
        }
    }
    return MOD_OK;
}

static t_module_ret_e _can_run(t_request_s *req)
{
    t_mmp_listelem_s *el;
    t_qhead_s *p;
    if (req==NULL || req->accept_encoding==NULL)
        return MOD_ERR;
    if (!is_mime_processable(req->mime_type))
        return MOD_ERR;
    for (el=req->accept_encoding->head; el!=NULL; el=el->next) {
        p = (t_qhead_s*)(el->data);
        if (p==NULL || p->id==NULL) {
            continue;
        }
        if (!strcmp("gzip", p->id))
            return MOD_OK;
    }
    return MOD_ERR;
}

static t_module_ret_e _on_prehead(t_response_s *res)
{
    long len;
    if ((len = _prelen(&res->rstate.sb))>=0)
        header_push_contentlength(res, len);
    header_push_contentencoding(res, "gzip");
    return MOD_PROCDONE;
}

typedef struct gzip_state_s {
    z_stream strm;
    unsigned char *in, *out;
    size_t out_written;
    int flush;
} t_gzip_state_s;
static void state_destroy(t_gzip_state_s **state)
{
    if (state==NULL || *state==NULL) return;
    if ((*state)->in!=NULL) xfree((*state)->in);
    if ((*state)->out!=NULL) xfree((*state)->out);
    xfree(*state);
    *state = NULL;
}
static t_gzip_state_s *state_create(t_response_s *res)
{
    t_gzip_state_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->in = ret->out = NULL;
    if (    ((ret->in = xmalloc(s_chunk_length))==NULL) ||
            ((ret->out = xmalloc(s_chunk_length))==NULL) ) {
        state_destroy(&ret);
        return NULL;
    }
    res->rstate.mod_res.data = ret;
    res->rstate.mod_res.data_len = sizeof(*ret)+s_chunk_length*2;
    return ret;
}
static t_gzip_state_s *state_get(t_response_s *res)
{
    return (t_gzip_state_s*)res->rstate.mod_res.data;
}

static t_module_ret_e _on_send(t_response_s *res)
{
    t_gzip_state_s *state;
    int ret;
    size_t written, have;
    state = state_get(res);
    printf("gzip on send.\n");
    if (state==NULL) {
        if ((state = state_create(res))==NULL) {
            printf("Could not create gzip state\n");
            return MOD_ERR;
        }
        state->strm.zalloc = Z_NULL;
        state->strm.zfree = Z_NULL;
        state->strm.opaque = Z_NULL;
        if ((ret = deflateInit2(&state->strm, Z_DEFAULT_COMPRESSION,
                            Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY))!=Z_OK)
            return MOD_ERR;
        do {
            ret = mmp_read(res->rstate.fd, state->in, s_chunk_length);
            state->flush = (ret==0) ? Z_FINISH : Z_NO_FLUSH;
            state->strm.next_in = state->in;
            state->strm.avail_in = ret;
            do {
                state->strm.avail_out = s_chunk_length;
                state->strm.next_out = state->out;
                deflate(&state->strm, state->flush);
                have = s_chunk_length - state->strm.avail_out;
                written = mmp_write(res->sock, state->out, have);
                if (written!=have) {
                    state->out_written = (written>0) ? written : 0;
                    return MOD_AGAIN;
                }
            } while(state->strm.avail_out==0);
        } while (state->flush!=Z_FINISH);
    } else {
        state = state_get(res);
        have = s_chunk_length - state->out_written;
        if (have>0) {
            written = mmp_write(res->sock, state->out+state->out_written, have);
            if (written!=have) {
                state->out_written += (written>0) ? written : 0;
                return MOD_AGAIN;
            }
        }
        do {
            ret = mmp_read(res->rstate.fd, state->in, s_chunk_length);
            state->flush = (ret==0) ? Z_FINISH : Z_NO_FLUSH;
            state->strm.next_in = state->in;
            state->strm.avail_in = ret;
            do {
                state->strm.avail_out = s_chunk_length;
                state->strm.next_out = state->out;
                deflate(&state->strm, state->flush);
                have = s_chunk_length - state->strm.avail_out;
                written = mmp_write(res->sock, state->out, have);
                if (written!=have) {
                    state->out_written = (written>0) ? written : 0;
                    return MOD_AGAIN;
                }
            } while(state->strm.avail_out==0);
        } while (state->flush!=Z_FINISH);
    }
    (void)deflateEnd(&state->strm);
    state_destroy(&state);
    return MOD_PROCDONE;
}

#ifdef MOD_GZIP_STATIC
t_module_s *mod_gzip_getmodule(void)
#else
OUTLINK t_module_s *getmodule(void)
#endif
{
    t_module_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    ret->name = "gzip";
    ret->set_params = _set_params;
    ret->init = _init;
    ret->fini = _fini;
    ret->can_run = _can_run;
    ret->on_accept = NULL;
    ret->on_presend = NULL;
    ret->on_prehead = _on_prehead;
    ret->on_send = _on_send;
    ret->on_postsend = NULL;
    ret->category = MODCAT_FILTER;
    return ret;
}

