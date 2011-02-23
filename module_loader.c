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
#include "module_loader.h"

#ifndef DISABLE_DYNAMIC
static char *get_module_filename(const char *name)
{
#ifdef _WIN32
    static const char *ext = ".dll";
    static const char *pre = "";
#else
    static const char *ext = ".so.1";
    static const char *pre = "lib";
#endif
    char *ret;
    if ((ret = xmalloc(strlen(pre)+strlen(name)+strlen(ext)+1))==NULL)
        return NULL;
    sprintf(ret, "%s%s%s", pre, name, ext);
    return ret;
}

ret_t module_loader_load(const t_config_s *params)
{
    t_mmp_listelem_s *p;
    t_module_s *mod;
    t_config_module_s *mod_conf;
    char *mod_filename;
    if (params==NULL || params->modules==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    for (p=params->modules->head; p!=NULL; p=p->next) {
        mod_conf = (t_config_module_s*)p->data;
        if (mod_conf==NULL || mod_conf->name==NULL)
            continue;
        mmp_trace_reset();
        printf("Loading module %s... ", mod_conf->name);
        if ((mod_filename = get_module_filename(mod_conf->name))==NULL) {
            printf("ENOMEM assigning name!?!\n");
            continue;
        }
        mod = module_add_dynamic(mod_filename);
        xfree(mod_filename);
        if (mod==NULL) {
            printf("ERR\n");
            mmp_trace_print(stdout);
            continue;
        }
        printf("Ok\n");
        if (mod->init!=NULL)
            mod->init();
        if (mod->set_params!=NULL)
            mod->set_params(mod_conf);
    }
    return MMP_ERR_OK;
}
#endif
