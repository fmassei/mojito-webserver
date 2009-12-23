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
#ifndef H_MJT_HEADERWQUALITY_H
#define H_MJT_HEADERWQUALITY_H

#include <mjt_types.h>
#include <lib/mjt_alloc.h>

/* I didn't want to write this. I swear. Anyway: extensions to the accept-like
 * headers, as in RFC2616-14.1 */
struct mjt_qhead_extp_s {
    char_t  *name;
    float_t quality;
    struct mjt_qhead_extp_s *next;
};

/* a struct for the "quality header". This header is simply a list of comma
 * separated string, plus an optional quality value expressed as a float
 * number, separated from each string with a semicolon. key[;q=D["."D+]][,...]
 * (the exact syntax is commented in each function, referring to RFC2616-14.3).
 * All this stuff is totaly unuseful in my opinion, but the HTTP/1.1 compliance
 * requires it. */
struct mjt_qhead_s {
    char_t  *id;
    float_t quality;
    struct mjt_qhead_extp_s *extp;
    struct mjt_qhead_s *next;
};

/* insert on the top of the queue. Hack for inserting despite of parsing. */
extern int_t mjt_qhead_insert(struct mjt_qhead_s **qhead,
                                struct mjt_qhead_s *newptr);
/* delete an entry */
extern void mjt_qhead_delete(struct mjt_qhead_s **qhead,
                                struct mjt_qhead_s **delptr);
/* parse the "quality header" */
extern struct mjt_qhead_s *mjt_qhead_parse(char_t *head);
/* frees the memory of a qhead struct */
extern void mjt_qhead_free(struct mjt_qhead_s **qhead);

#endif /* H_MJT_HEADERWQUALITY_H */
