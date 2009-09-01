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

#include "filter.h"
#include <dlfcn.h>

struct module_filter_s *filter_modules = NULL;

int filter_init()
{
    struct module_filter_s *p;
    int ret = 0;
    for (p=filter_modules; p!=NULL; p=p->next)
        if (p->base.module_init!=NULL)
            ret |= p->base.module_init();
    return ret;
}

int filter_fini()
{
    struct module_filter_s *p;
    int ret = 0;
    for (p=filter_modules; p!=NULL; p=p->next)
        if (p->base.module_fini!=NULL)
            ret |= p->base.module_fini();
    return ret;
}

struct module_filter_s *filter_add_static_mod(
                                    struct module_filter_s*(*get_module)(void))
{
    struct module_filter_s *p;
    if ((p = get_module())==NULL)
        return NULL;
    p->next = filter_modules;
    filter_modules = p;
    return p;
}

#ifdef DYNAMIC
struct module_filter_s *filter_add_dynamic_mod(char *fname, char **error)
{
    struct module_filter_s *(*get_module)(void);
    void *handle;
    if ((handle = dlopen(fname, RTLD_NOW | RTLD_GLOBAL))==NULL) {
        *error = dlerror();
        return NULL;
    }
    dlerror();
    *(void**)(&get_module) = dlsym(handle, "getmodule");
    if ((*error = dlerror())!=NULL)
        return NULL;
    return filter_add_static_mod(get_module);
}
#endif /* DYNAMIC */

