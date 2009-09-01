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

#ifndef H_PLIST_H
#define H_PLIST_H

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct plist_s {
    char *key;
    char *value;
    struct plist_s *next;
};

void plist_destroy(struct plist_s **plist);
int plist_insert(struct plist_s **plist, char *key, char *value, int checkdup);
char *plist_search(struct plist_s *plist, char *key);

#endif /* H_PLIST_H */
