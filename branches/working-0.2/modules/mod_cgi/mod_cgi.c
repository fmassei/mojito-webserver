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

#include "mod_cgi.h"

static int _on_presend(int sock, struct request_s *req)
{
    if (access(req->abs_filename, X_OK)==0) {
        cgi_run(req->abs_filename, sock);
        /* this should not happen */
        return MOD_ALLDONE;
    }
    return MOD_OK;
}

#ifdef MODULE_STATIC
struct module_s *mod_cgi_getmodule()
#else
struct module_s *getmodule()
#endif
{
    struct module_s *p;
    if ((p = malloc(sizeof(*p)))==NULL)
        return NULL;
    p->name = "cgi";
    p->set_params = NULL;
    p->init = NULL;
    p->fini = NULL;
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

