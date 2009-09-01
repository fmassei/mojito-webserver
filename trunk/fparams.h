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

#ifndef H_FPARAMS_H
#define H_FPARAMS_H

#define _BSD_SOURCE 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <libgen.h>
#include "plist.h"

struct module_params_s {
    char *name;
    struct plist_s *params;
    struct module_params_s *next;
};

/* server parameters */
typedef struct fparams_s {
    char *pidfile;
    char *http_root;
    char *default_page;
    char *tmp_dir;
    int uid, gid;
    int listen_port, listen_queue;
    int keepalive_timeout;
    long min_compress_filesize;
    char *server_meta;
    char *module_basepath;
    struct module_params_s *mod_params;
} fparams_st;

/* parse an INI file */
int params_loadFromINIFile(const char *fname, fparams_st *params);
/* free the parameters */
void params_free(fparams_st *params);
/* return the given parameter module (if any) */
struct module_params_s *params_getModuleParams(fparams_st *params, char *name);

#endif /* H_FPARAMS_H */
