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
#ifndef HEADER_W_QUALITY
#define HEADER_W_QUALITY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mmp/mmp_string.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_memory.h>
#include <mmp/mmp_list.h>

/* I didn't want to write this. I swear. Anyway: extensions to the accept-like
 * headers, as in RFC2616-14.1 */
typedef struct extp_s {
    char *name;
    float quality;
} t_extp_s;

/* a struct for the "quality header". This header is simply a list of comma
 * separated string, plus an optional quality value expressed as a float
 * number, separated from each string with a semicolon. key[;q=D["."D+]][,...]
 * (the exact syntax is commented in each function, referring to RFC2616-14.3).
 * All this stuff is totaly unuseful in my opinion, but the HTTP/1.1 compliance
 * requires it. */
typedef struct qhead_s {
    char *id;
    float quality;
    t_mmp_list_s *extp_list;
} t_qhead_s;

typedef t_mmp_list_s t_qhead_list_s;

/* create a qhead */
t_qhead_s *qhead_create(const char *id);
/* destroy a qhead */
void qhead_destroy(t_qhead_s **qhead);

/* insert on the top of the queue. Hack for inserting despite of parsing. */
ret_t qhead_list_insert(t_qhead_list_s *qhead_list, t_qhead_s *qhead);
/* delete an entry */
void qhead_list_delete(t_qhead_list_s *qhead_list, t_qhead_s **e2d);
/* parse the "quality header" */
t_qhead_list_s *qhead_list_parse(const char *head);
/* frees the memory of a qhead struct */
void qhead_list_destroy(t_qhead_list_s **qhead_list);

#ifdef UNIT_TESTING
ret_t test_qhead_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* HEADER_W_QUALITY */
