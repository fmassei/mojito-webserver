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
#include "header_w_quality.h"
#include "logger/logger.h"

/* define a filter worker type - out(as buffer), in (as fd), size */
typedef int (*filter_ft)(unsigned char *, int, ssize_t);
/* define a pre-calculation function for output lenght. If the length can not
 * be calculated (as normally happens) a negative number should be returned */
typedef ssize_t (*filter_len_ft)(struct stat *);

/* filter struct */
struct filter_s {
    char*           name;
    filter_ft       worker;
    filter_len_ft   prelength;
    struct filter_s *next;
};

/* filter functions */
int filter_init();
struct filter_s *filter_register(char *name, filter_ft worker,
                                                        filter_len_ft prelen);
int filter_sanitize_queue(struct qhead_s **qhead);
int filter_is_present(struct qhead_s *qhead, char *id);
struct filter_s *filter_findfilter(struct qhead_s *qhead);
void filter_free();

#endif /* H_FILTER_H */
