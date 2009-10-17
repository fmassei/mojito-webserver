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

#ifdef DYNAMIC
/* get the library full path given the basename and the libname */
static char *getlibname(struct fparam_s *params, char *basename, char *libname)
{
    char *lname;
    size_t nlen;
    nlen = strlen(params->module_basepath)+strlen(basename)+strlen(libname)+
            strlen("/lib.so.1")+1;
    if ((lname = malloc(nlen))==NULL)
        return NULL;
    sprintf(lname, "%s/lib%s%s.so.1", params->module_basepath,
                                        basename,
                                        libname);
    return lname;
}
#endif /* DYNAMIC */

#ifdef DYNAMIC_MODULE
static int load_dynamic_module(struct fparam_s *prm,
                    struct module_params_s *mpars, char *modname, char **error)
{
    struct module_s *mod;
    struct plist_s *lpars;
    char *buf;
    *error = modname;
    if ((buf = getlibname(prm, "", modname))==NULL) {
        *error = "Couldn't get libname for module";
        return -1;
    }
    if ((mod = module_add_dynamic(buf, error))==NULL) {
        free(buf);
        return -1;
    }
    if (buf!=NULL)
        free(buf);
    if (mod->set_params!=NULL) {
        if (mpars==NULL) {
            logmsg(LOG_WARNING, "No parameters found for %s", modname);
            lpars = NULL;
        } else {
            lpars = mpars->params;
        }
        if (mod->set_params(lpars)<0) {
            *error = "Failed passing parameters to module";
            return -2;
        }
    }
    return 0;
}
#else
static int load_static_module(struct module_params_s *mpars,
                        struct module_s*(*modfnc)(void), char **error)
{
    struct module_s *mod;
    struct plist_s *lpars;
    if ((mod = module_add_static(modfnc))==NULL) {
        *error = "Error loading static module";
        return -1;
    }
    if (mod->set_params!=NULL) {
        if (mpars==NULL) {
            /* FIXME: found module name */
            logmsg(LOG_WARNING, "No parameters found for module");
            lpars = NULL;
        } else {
            lpars = mpars->params;
        }
        if (mod->set_params(lpars)<0) {
            *error = "Failed passing parameters to module";
            return -2;
        }
    }
    return 0;
}
#endif

/* get the filter module(s) */
int module_get(struct fparam_s *prm, char **error)
{
    struct module_params_s *mpars;
    int err;
#ifdef DYNAMIC_MODULE
    char *modname;
    int i;
#else
    extern struct module_s          *mod_stat_getmodule(void),
                                    *mod_cacheshm_getmodule(void),
                                    *mod_identity_getmodule(void),
                                    *mod_gzip_getmodule(void),
                                    *mod_deflate_getmodule(void),
                                    *mod_cgi_getmodule(void),
                                    *mod_fcgi_getmodule(void);
#endif
    if ((mpars = params_getModuleParams(prm, "modules"))==NULL) {
        *error = "section [modules] not found in config.ini";
        return -1;
    }
#ifdef DYNAMIC_MODULE
    if ((modname = plist_search(mpars->params, "module"))==NULL) {
        *error = "value for \"module\" not found in section [modules]";
        return -1;
    }
    while (*modname!='\0') {
        while (*modname==' ') modname++;
        /* TODO fix ugly ugly ugly */
        for(i=0; (*(modname+i)>='a' && *(modname+i)<='z')||*(modname+i)=='_'; ++i) ;
        *(modname+i) = '\0';
        mpars = params_getModuleParams(prm, modname);
        if ((err = load_dynamic_module(prm, mpars, modname, error))<0)
            return err;
        modname += i+1;
    }
#else
    mpars = params_getModuleParams(prm, "mod_stat");
    if ((err = load_static_module(mpars, mod_stat_getmodule, error))<0)
        return err;
    mpars = params_getModuleParams(prm, "mod_cacheshm");
    if ((err = load_static_module(mpars, mod_cacheshm_getmodule, error))<0)
        return err;
    mpars = params_getModuleParams(prm, "mod_gzip");
    if ((err = load_static_module(mpars, mod_gzip_getmodule, error))<0)
        return err;
    mpars = params_getModuleParams(prm, "mod_deflate");
    if ((err = load_static_module(mpars, mod_deflate_getmodule, error))<0)
        return err;
    mpars = params_getModuleParams(prm, "mod_identity");
    if ((err = load_static_module(mpars, mod_identity_getmodule, error))<0)
        return err;
    mpars = params_getModuleParams(prm, "mod_cgi");
    if ((err = load_static_module(mpars, mod_cgi_getmodule, error))<0)
        return err;
    mpars = params_getModuleParams(prm, "mod_fcgi");
    if ((err = load_static_module(mpars, mod_fcgi_getmodule, error))<0)
        return err;
#endif
    return 0;
}

