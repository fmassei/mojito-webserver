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

#ifndef H_MJT_PLIST_H
#define H_MJT_PLIST_H

#include <mjt_types.h>

BEGIN_C_DECLS

struct kvlist_s {
    char_t *key;
    char_t *val;
    struct kvlist_s *next;
};

extern void mjt_kvlist_destroy(struct kvlist_s **ptr);
extern int mjt_kvlist_insert(struct kvlist_s **ptr, char_t *key, char_t *val,
                             int_t checkdup);
extern char_t *mjt_kvlist_search(struct kvlist_s *ptr, char_t *key);

END_C_DECLS

#endif /* H_MJT_PLIST_H */
