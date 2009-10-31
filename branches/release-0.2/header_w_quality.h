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

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* I didn't want to write this. I swear. Anyway: extensions to the accept-like
 * headers, as in RFC2616-14.1 */
struct extp_s {
    char *name;
    float quality;
    struct extp_s *next;
};

/* a struct for the "quality header". This header is simply a list of comma
 * separated string, plus an optional quality value expressed as a float
 * number, separated from each string with a semicolon. key[;q=D["."D+]][,...]
 * (the exact syntax is commented in each function, referring to RFC2616-14.3).
 * All this stuff is totaly unuseful in my opinion, but the HTTP/1.1 compliance
 * requires it. */
struct qhead_s {
    char *id;
    float quality;
    struct extp_s *extp;
    struct qhead_s *next;
};

/* insert on the top of the queue. Hack for inserting despite of parsing. */
int qhead_insert(struct qhead_s **qhead, struct qhead_s *p);
/* delete an entry */
void qhead_delete(struct qhead_s **qhead, struct qhead_s *e2d);
/* parse the "quality header" */
struct qhead_s *qhead_parse(char *head);
/* frees the memory of a qhead struct */
void qhead_free(struct qhead_s *qhead);

#endif /* HEADER_W_QUALITY */
