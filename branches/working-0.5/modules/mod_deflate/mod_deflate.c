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
#include "../../modules.h"
#include <zlib.h>

#include <sys/sendfile.h>

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

static t_module_ret_e _can_run(t_request_s *req)
{
    t_mmp_listelem_s *el;
    t_qhead_s *p;
    if (req==NULL || req->accept_encoding==NULL)
        return MOD_ERR;
    for (el=req->accept_encoding->head; el!=NULL; el=el->next) {
        p = (t_qhead_s*)(el->data);
        if (p==NULL || p->id==NULL) {
            continue;
        }
        if (!strcmp("deflate", p->id))
            return MOD_OK;
    }
    return MOD_ERR;
}

typedef void(*t_hpclfptr)(t_response_s*,long);
typedef void(*t_hpcefptr)(t_response_s*,char*);
static t_hpclfptr _gethpcl(void)
{
#ifndef _WIN32
    return header_push_contentlength;
#else
    return (t_hpclfptr)GetProcAddress(GetModuleHandle(NULL),
                                        "header_push_contentlength");
#endif
}
static t_hpcefptr _gethpce(void)
{
#ifndef _WIN32
    return header_push_contentencoding;
#else
    return (t_hpcefptr)GetProcAddress(GetModuleHandle(NULL),
                                        "header_push_contentencoding");
#endif
}

static t_module_ret_e _on_prehead(t_response_s *res)
{
    long len;
    if ((len = _prelen(&res->rstate.sb))>=0)
        (*_gethpcl())(res, len);
    (*_gethpce())(res, "deflate");
    return MOD_PROCDONE;
}

#define CHUNK_LEN (16*1024)  /* TODO move to config (see notes) */

typedef struct deflate_state_s {
    z_stream strm;
    unsigned char *in, *out;
    size_t out_written;
    int flush;
} t_deflate_state_s;
static void state_destroy(t_deflate_state_s **state)
{
    if (state==NULL || *state==NULL) return;
    if ((*state)->in!=NULL) xfree((*state)->in);
    if ((*state)->out!=NULL) xfree((*state)->out);
    xfree(*state);
    *state = NULL;
}
static t_deflate_state_s *state_create(t_response_s *res)
{
    t_deflate_state_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->in = ret->out = NULL;
    if (    ((ret->in = xmalloc(CHUNK_LEN))==NULL) ||
            ((ret->out = xmalloc(CHUNK_LEN))==NULL) ) {
        state_destroy(&ret);
        return NULL;
    }
    res->rstate.mod_res.data = ret;
    res->rstate.mod_res.data_len = sizeof(*ret)+CHUNK_LEN*2;
    return ret;
}
static t_deflate_state_s *state_get(t_response_s *res)
{
    return (t_deflate_state_s*)res->rstate.mod_res.data;
}

static t_module_ret_e _on_send(t_response_s *res)
{
    t_deflate_state_s *state;
    int ret;
    size_t written, have;
    state = state_get(res);
    if (state==NULL) {
        state = state_create(res);
        state->strm.zalloc = Z_NULL;
        state->strm.zfree = Z_NULL;
        state->strm.opaque = Z_NULL;
        if ((ret = deflateInit(&state->strm, Z_DEFAULT_COMPRESSION))!=Z_OK)
            return MOD_ERR;
        do {
            ret = mmp_read(res->rstate.fd, state->in, CHUNK_LEN);
            state->flush = (ret==0) ? Z_FINISH : Z_NO_FLUSH;
                return MOD_PROCDONE;
            state->strm.next_in = state->in;
            state->strm.avail_in = ret;
            do {
                state->strm.avail_out = CHUNK_LEN;
                state->strm.next_out = state->out;
                deflate(&state->strm, state->flush);
                have = CHUNK_LEN - state->strm.avail_out;
                written = mmp_write(res->sock, state->out, have);
                if (written!=have) {
                    state->out_written = (written>0) ? written : 0;
                    return MOD_AGAIN;
                }
            } while(state->strm.avail_out==0);
        } while (state->flush!=Z_FINISH);
    } else {
        state = state_get(res);
        have = CHUNK_LEN - state->out_written;
        if (have>0) {
            written = mmp_write(res->sock, state->out+state->out_written, have);
            if (written!=have) {
                state->out_written += (written>0) ? written : 0;
                return MOD_AGAIN;
            }
        }
        do {
            ret = mmp_read(res->rstate.fd, state->in, CHUNK_LEN);
            state->flush = (ret==0) ? Z_FINISH : Z_NO_FLUSH;
                return MOD_PROCDONE;
            state->strm.next_in = state->in;
            state->strm.avail_in = ret;
            do {
                state->strm.avail_out = CHUNK_LEN;
                state->strm.next_out = state->out;
                deflate(&state->strm, state->flush);
                have = CHUNK_LEN - state->strm.avail_out;
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

#ifdef MODULE_STATIC
t_module_s *mod_deflate_getmodule(void)
#else
OUTLINK t_module_s *getmodule(void)
#endif
{
    t_module_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    ret->name = "deflate";
    ret->set_params = NULL;
    ret->init = NULL;
    ret->fini = NULL;
    ret->can_run = _can_run;
    ret->on_accept = NULL;
    ret->on_presend = NULL;
    ret->on_prehead = _on_prehead;
    ret->on_send = _on_send;
    ret->on_postsend = NULL;
    ret->will_run = 1;
    ret->category = MODCAT_FILTER;
    return ret;
}

