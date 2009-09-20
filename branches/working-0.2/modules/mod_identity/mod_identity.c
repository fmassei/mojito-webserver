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

#include "../modules.h"
#include <stdio.h>
#include <stdlib.h>

static int _compress(unsigned char *addr, int fd, ssize_t len)
{
    if (write(fd, addr, len)!=len)
        return -1;
    return 0;
}

static ssize_t _prelen(struct stat *sb)
{
    if (sb->st_size==0)
        return 0;
    return -1;
}

static int _can_run(struct request_s *req)
{
    struct qhead_s *p;
    for (p=req->header.accept_encoding; p!=NULL; p=p->next)
        if (!strcmp("identity", p->id))
            return MOD_OK;
    return MOD_ERR;
}

static int _on_prehead(struct stat *sb)
{
    ssize_t len;
    if ((len = _prelen(sb))>=0)
        header_push_contentlength(len);
    header_push_contentencoding("identity");
    return MOD_OK;
}

static int _on_send(void *addr, int sock, struct stat *sb)
{
    extern char *ch_filter;
    ch_filter = "identity";
    if (_compress(addr, sock, sb->st_size)!=0)
        return MOD_CRIT;
    return MOD_PROCDONE;
}

#ifdef MODULE_STATIC
struct module_s *mod_identity_getmodule()
#else
struct module_s *getmodule()
#endif
{
    struct module_s *p;
    if ((p = malloc(sizeof(*p)))==NULL)
        return NULL;
    p->name = "identity";
    p->set_params = NULL;
    p->init = NULL;
    p->fini = NULL;
    p->can_run = _can_run;
    p->on_accept = NULL;
    p->on_presend = NULL;
    p->on_prehead = _on_prehead;
    p->on_send = _on_send;
    p->on_postsend = NULL;
    p->next = p->prev = NULL;
    p->will_run = 1;
    p->category = MODCAT_FILTER;
    return p;
}

