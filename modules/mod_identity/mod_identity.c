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
#include "../../modules.h"

/* TODO: missing parameter checks! */

static int _compress(unsigned char *addr, int fd, int len)
{
    if (mmp_write(fd, addr, (unsigned int)len)!=len)
        return -1;
    return 0;
}

static int _prelen(t_mmp_stat_s *sb)
{
    if (sb->st_size==0)
        return 0;
    return -1;
}

static int _can_run(t_request_s *req)
{
    t_mmp_listelem_s *el;
    t_qhead_s *p;
    if (req==NULL || req->accept_encoding==NULL)
        return MOD_ERR;
    for (el=req->accept_encoding->head; el!=NULL; el=el->next) {
        p = (t_qhead_s*)(el->data);
        if (!strcmp("identity", p->id))
            return MOD_OK;
    }
    return MOD_ERR;
}

static int _on_prehead(t_mmp_stat_s *sb, t_response_s *res)
{
    size_t len;
    if ((len = _prelen(sb))>=0)
        header_push_contentlength(res, len);
    header_push_contentencoding(res, "identity");
    return MOD_PROCDONE;
}

static int _on_send(void *addr, t_mmp_stat_s *sb, t_response_s *res)
{
    if (_compress(addr, res->sock, sb->st_size)!=0)
        return MOD_CRIT;
    return MOD_PROCDONE;
}

#ifdef MODULE_STATIC
t_module_s *mod_identity_getmodule()
#else
t_module_s *getmodule()
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
    ret->will_run = 1;
    ret->category = MODCAT_FILTER;
    return ret;
}

