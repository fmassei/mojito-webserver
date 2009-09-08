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

static int _on_accept(void)
{
    DEBUG_LOG((LOG_DEBUG, "Accepted."));
    return 0;
}

#ifdef MODULE_STATIC
struct module_s *mod_stat_getmodule()
#else
struct module_s *getmodule()
#endif
{
    struct module_s *p;
    if ((p = malloc(sizeof(*p)))==NULL)
        return NULL;
    p->init = NULL;
    p->fini = NULL;
    p->set_params = NULL;
    p->on_accept = _on_accept;
    p->on_presend = NULL;
    p->on_postsend = NULL;
    p->next = NULL;
    return p;
}

