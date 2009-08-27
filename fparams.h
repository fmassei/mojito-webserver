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

/* server parameters */
typedef struct fparams_s {
    char *logfile;
    char *errfile;
    char *pidfile;
    char *http_root;
    char *default_page;
    char *tmp_dir;
    char *cache_dir;
    int uid, gid;
    int listen_port, listen_queue;
    int keepalive_timeout;
    long min_compress_filesize;
    char *server_meta;
#ifdef DYNAMIC
    char *module_basepath;
#ifdef DYNAMIC_LOGGER
    char *module_logger;
#endif /* DYNAMIC_LOGGER */
#ifdef DYNAMIC_CACHE
    char *module_cache;
#endif /* DYNAMIC_CACHE */
#endif /* DYNAMIC */
} fparams_st;

/* parse an INI file */
int params_loadFromINIFile(const char *fname, fparams_st *params);
/* free the parameters */
void params_free(fparams_st *params);

#endif /* H_FPARAMS_H */
