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
#include "logger.h"

typedef struct static_mod_def_s {
    const char *name;
    t_get_module_f fnc;
} t_static_mod_def_s;

#ifdef MOD_IDENTITY_STATIC
extern t_module_s *mod_identity_getmodule(void);
#endif
#ifdef MOD_DEFLATE_STATIC
extern t_module_s *mod_deflate_getmodule(void);
#endif
#ifdef MOD_GZIP_STATIC
extern t_module_s *mod_gzip_getmodule(void);
#endif
#ifdef MOD_CGI_STATIC
extern t_module_s *mod_cgi_getmodule(void);
#endif

static t_static_mod_def_s s_static_mod_defs[] = {
    #ifdef MOD_IDENTITY_STATIC
        { "mod_identity", mod_identity_getmodule },
    #endif 
    #ifdef MOD_DEFLATE_STATIC
        { "mod_deflate", mod_deflate_getmodule },
    #endif
    #ifdef MOD_GZIP_STATIC
        { "mod_gzip", mod_gzip_getmodule },
    #endif
    #ifdef MOD_CGI_STATIC
        { "mod_cgi", mod_cgi_getmodule },
    #endif
};
#define N_STATIC_MOD_DEFS \
            (sizeof(s_static_mod_defs)/sizeof(s_static_mod_defs[0]))

#ifndef DISABLE_DYNAMIC_MODULES
static char *get_module_filename(const char *name)
{
#if defined(_WIN32) || defined(__CYGWIN__)
    static const char *ext = "-0.dll";
    static const char *pre = "";
#   define SLASH "\\"
#else
    static const char *ext = ".so";
    static const char *pre = "";
#   define SLASH "/"
#endif
    const t_config_s *cfg = config_get();
    char *ret;
    if ((ret = xmalloc(strlen(cfg->server->modules_basepath)+1+
            strlen(pre)+strlen(name)+strlen(ext)+1))==NULL)
        return NULL;
    sprintf(ret, "%s%s%s%s%s", cfg->server->modules_basepath, SLASH,
        pre, name, ext);
    return ret;
#undef SLASH
}
#endif /* DISABLE_DYNAMIC_MODULES */

ret_t module_loader_load(const t_config_s *params)
{
    t_mmp_listelem_s *p;
    t_module_s *mod;
    t_config_module_s *mod_conf;
    char *mod_filename;
    int i;
    if (params==NULL || params->modules==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    for (p=params->modules->head; p!=NULL; p=p->next) {
        mod_conf = (t_config_module_s*)p->data;
        if (mod_conf==NULL || mod_conf->name==NULL)
            continue;
        mmp_trace_reset();
        for (i=0; i<N_STATIC_MOD_DEFS; ++i) {
            if (!strcmp(s_static_mod_defs[i].name, mod_conf->name)) {
                mod = module_add_static(s_static_mod_defs[i].fnc);
                if (mod==NULL) {
                    log_err(LOGTYPE_ERR, "Error loading module %s [static]",
                        mod_conf->name);
                    mmp_trace_print(stdout);
                    continue;
                }
                log_err(LOGTYPE_INFO, "Loaded module %s [static]",
                        mod_conf->name);
                goto got_it;
            }
        }
#ifndef DISABLE_DYNAMIC_MODULES
        if ((mod_filename = get_module_filename(mod_conf->name))==NULL) {
            log_err(LOGTYPE_ERR, "Error loading module %s [dynamic]",
                    mod_conf->name);
            continue;
        }
        mod = module_add_dynamic(mod_filename);
        xfree(mod_filename);
        if (mod==NULL) {
            log_err(LOGTYPE_ERR, "Error loading module %s [dynamic]",
                    mod_conf->name);
            mmp_trace_print(stdout);
            continue;
        }
        log_err(LOGTYPE_INFO, "Loaded module %s [dynamic]",
                mod_conf->name);
#else /* DISABLE_DYNAMIC_MODULES */
        log_err(LOGTYPE_ERR, "Module %s not found [dynamic]",
                mod_conf->name);
        continue;
#endif /* DISABLE_DYNAMIC_MODULES */
got_it:
        if (mod->init!=NULL)
            mod->init();
        if (mod->set_params!=NULL)
            mod->set_params(mod_conf);
    }
    return MMP_ERR_OK;
}

