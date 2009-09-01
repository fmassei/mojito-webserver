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

#include "module.h"
#include "cache/cache.h"
#include "logger/logger.h"

#ifdef DYNAMIC
/* get the library full path given the basename and the libname */
static char *getlibname(fparams_st *params, char *basename, char *libname)
{
    char *lname;
    size_t nlen;
    nlen = strlen(params->module_basepath)+strlen(basename)+strlen(libname)+
            strlen("lib.so.1")+1;
    if ((lname = malloc(nlen))==NULL)
        return NULL;
    sprintf(lname, "%s/lib%s%s.so.1", params->module_basepath,
                                        basename,
                                        libname);
    return lname;
}
#endif /* DYNAMIC */

/* get the cache module */
int module_get_cache(fparams_st *prm)
{
#ifdef DYNAMIC_CACHE
    char *buf, *error;
    if (prm->module_cache!=NULL) {
        if ((buf = getlibname(prm, "cache", prm->module_cache))==NULL)
            return -1;
        if (cache_add_dynamic_mod(buf, &error)<0) {
            fprintf(stderr, "%s\n", error);
            free(buf);
            return -1;
        }
    }
    if (buf!=NULL)
        free(buf);
#else /* DYNAMIC_CACHE */
    extern struct module_cache_s *shm_getmodule(void);
    if (cache_add_static_mod(shm_getmodule)) {
        fprintf(stderr, "Error loading static cache module\n");
        return -1;
    }
#endif /* DYNAMIC_CACHE */
    cache_set_global_parameters(prm);
    return 0;
}

/* get the cache module */
int module_get_logger(fparams_st *prm)
{
#ifdef DYNAMIC_LOGGER
    char *buf, *error;
    if (prm->module_logger!=NULL) {
        if ((buf = getlibname(prm, "logger", prm->module_logger))==NULL)
            return -1;
        if (logger_add_dynamic_mod(buf, &error)<0) {
            fprintf(stderr, "%s\n", error);
            free(buf);
            return -1;
        }
    }
    if (buf!=NULL)
        free(buf);
#else /* DYNAMIC_CACHE */
    extern struct module_logger_s *std_getmodule(void);
    if (logger_add_static_mod(std_getmodule)) {
        fprintf(stderr, "Error loading static logger module\n");
        return -1;
    }
#endif /* DYNAMIC_CACHE */
    logger_set_global_parameters(prm);
    return 0;
}

