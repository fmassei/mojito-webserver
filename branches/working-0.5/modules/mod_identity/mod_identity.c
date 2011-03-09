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

#include <stdio.h>
#include <stdlib.h>
#include <mmp/mmp_memory.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_socket.h>
#include "../../common_src/resp_headers.h"
#include "../../src/modules.h"
#include "../../src/utils.h"

#ifdef HAVE_SYS_SENDFILE_H
#   include <sys/sendfile.h>
#endif /* HAVE_SYS_SENDFILE_H */

/* TODO: missing parameter checks! */

#ifdef _WIN32
#   define OUTLINK  __declspec(dllexport)
#else
#   define OUTLINK
#endif

static int _prelen(t_mmp_stat_s *sb)
{
    if (sb->st_size>=0)
        return sb->st_size;
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
        if (!strcmp("identity", p->id))
            return MOD_OK;
    }
    return MOD_ERR;
}

static t_module_ret_e _on_prehead(t_response_s *res)
{
    long len;
    if ((len = _prelen(&res->rstate.sb))>=0)
        header_push_contentlength(res, len);
    header_push_contentencoding(res, "identity");
    return MOD_PROCDONE;
}

static t_module_ret_e _on_send(t_response_s *res)
{
    ssize_t ret;
#ifdef HAVE_SENDFILE
    DBG_PRINT(("[mod_identity] sendfile in\n"));
    ret = sendfile(res->sock, res->rstate.fd, NULL,
                            res->rstate.sb.st_size-res->rstate.sent);
    DBG_PRINT(("[mod_identity] sendfile out %d\n", ret));
#else
    /* TODO: missing sendfile alternative */
    ret = -1;
#endif
    if (ret<0) {
        if (errno==EAGAIN)
            return MOD_AGAIN;
        else
            return MOD_ERR;
    }
    res->rstate.sent += ret;
    res->final_data_sent = res->rstate.sent;
    if (res->rstate.sent>=res->rstate.sb.st_size)
        return MOD_PROCDONE;
    return MOD_AGAIN;
}

#ifdef MOD_IDENTITY_STATIC
t_module_s *mod_identity_getmodule(void)
#else
OUTLINK t_module_s *getmodule(void)
#endif
{
    t_module_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    ret->name = "identity";
    ret->set_params = NULL;
    ret->init = NULL;
    ret->fini = NULL;
    ret->can_run = _can_run;
    ret->on_accept = NULL;
    ret->on_presend = NULL;
    ret->on_prehead = _on_prehead;
    ret->on_send = _on_send;
    ret->on_postsend = NULL;
    ret->category = MODCAT_FILTER;
    return ret;
}

