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

#include "mod_fcgi.h"

/* number of pre-created socket pairs (aka max runnable fcgi apps) */
#define POOL_SIZE    10

struct la_s *la;

static int _on_init(void)
{
    int i;
    struct fcgi_app *p;
    if ((la = lacreate(sizeof(struct fcgi_app*)*POOL_SIZE))==NULL) {
        return MOD_CRIT;
    }
    la->b = 1;
    memset((char*)laget(la), 0, sizeof(struct fcgi_app*)*POOL_SIZE);
    la->b = 0;
    return MOD_OK;
}

static int _on_fini(void)
{
    ladestroy(la);
    return MOD_OK;
}

static int _on_presend(int sock, struct request_s *req)
{
    if (access(req->abs_filename, X_OK)==0) {
        fcgi_run(req, sock);
        /* FIXME: this should be MOD_ALLDONE and signal to keep alive */
        return MOD_CRIT;
    }
    return MOD_OK;
}

#ifdef MODULE_STATIC
struct module_s *mod_fcgi_getmodule()
#else
struct module_s *getmodule()
#endif
{
    struct module_s *p;
    if ((p = malloc(sizeof(*p)))==NULL)
        return NULL;
    p->name = "fcgi";
    p->set_params = NULL;
    p->init = _on_init;
    p->fini = _on_fini;
    p->can_run = NULL;
    p->on_accept = NULL;
    p->on_presend = _on_presend;
    p->on_prehead = NULL;
    p->on_send = NULL;
    p->on_postsend = NULL;
    p->next = p->prev = NULL;
    p->will_run = 1;
    p->category = MODCAT_DYNAMIC;
    return p;
}

