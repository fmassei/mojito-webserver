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

#ifndef H_FILTER_H
#define H_FILTER_H

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../module.h"

struct module_filter_s {
    struct module_fnc_s base;
    int (*compress)(unsigned char*, int, ssize_t);
    ssize_t (*prelen)(struct stat *);
    struct module_filter_s *next;
};

int filter_init();
int filter_fini();

struct module_filter_s *filter_add_static_mod(
                                    struct module_filter_s *(get_module)(void));
struct module_filter_s *filter_add_dynamic_mod(char *fname, char **error);

#endif /* H_FILTER_H */
