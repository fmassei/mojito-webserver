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
#include "modules.h"

static struct module_s *modules = NULL;

/* TODO FIXME XXX better check of the return values!!!! */
int mod_set_params(struct plist_s *params)
{
    struct module_s *p;
    int ret = 0;
    for (p=modules; p!=NULL; p=p->next)
        if (p->set_params!=NULL)
            if (p->set_params(params)!=0)
                ret--;
    return ret;
}

int mod_init(void)
{
    struct module_s *p;
    int ret = 0;
    for (p=modules; p!=NULL; p=p->next)
        if (p->init!=NULL)
            if (p->init()!=0)
                ret--;
    return ret;
}

int mod_fini(void)
{
    struct module_s *p;
    int ret = 0;
    for (p=modules; p!=NULL; p=p->next)
        if (p->fini!=NULL)
            if (p->fini()!=0)
                ret--;
    return ret;
}

int on_accept(void)
{
    struct module_s *p;
    int ret = 0;
    for (p=modules; p!=NULL; p=p->next)
        if (p->on_accept!=NULL)
            if (p->on_accept()!=0)
                ret--;
    return ret;
}

int on_presend(int sock, struct request_s *req)
{
    struct module_s *p;
    int ret = 0;
    for (p=modules; p!=NULL; p=p->next)
        if (p->on_presend!=NULL)
            if (p->on_presend(sock, req)!=0)
                ret--;
    return ret;
}

int on_postsend(struct request_s *req, struct module_filter_s *filter,
                                            char *mime, void *addr, size_t size)
{
    struct module_s *p;
    int ret = 0;
    for (p=modules; p!=NULL; p=p->next)
        if (p->on_postsend!=NULL)
            if (p->on_postsend(req, filter, mime, addr, size)!=0)
                ret--;
    return ret;
}

struct module_s *module_add_static(struct module_s *(*get_module)(void))
{
    struct module_s *p;
    if ((p = get_module())==NULL)
        return NULL;
    p->next = modules;
    modules = p;
    return p;
}

#ifdef DYNAMIC
struct module_s *module_add_dynamic(char *fname, char **error)
{
    struct module_s*(*get_module)(void);
    void *handle;
    if ((handle = dlopen(fname, RTLD_NOW | RTLD_GLOBAL))==NULL) {
        *error = dlerror();
        return NULL;
    }
    dlerror();
    *(void**)(&get_module) = dlsym(handle, "getmodule");
    if ((*error = dlerror())!=NULL)
        return NULL;
    return module_add_static(get_module);
}
#endif /* DYNAMIC */

