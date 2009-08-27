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
#ifndef H_CACHE_H
#define H_CACHE_H

#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../fparams.h"
#include "../logger/logger.h"

/* Ok. Here it is our URI cache.
 * Why we need a cache in the first place? That's tricky. We can't know the
 * size of the transmitted payload when it passes through a filter, but without
 * a content-length we can't have persistent connections (aka keep-alive). */

/* entry struct */
struct cache_entry_s {
    char *URI;
    char *fname;
    char *filter_id;
    char *content_type;
};

/* init/fini */
int cache_init();
void cache_fini();
/* set global parameters */
void cache_set_global_parameters(fparams_st *params);
/* lookup */
struct cache_entry_s *cache_lookup(const char *URI, const char *filter_id);
/* create and install */
int cache_create_file(const char *URI, char *filter_id, char *content_type);

#endif /* H_CACHE_H */
