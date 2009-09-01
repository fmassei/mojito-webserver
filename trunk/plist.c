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

#include "plist.h"

static void plist_destroy_node(struct plist_s *plist)
{
    if (plist==NULL) return;
    if (plist->key!=NULL)
        free(plist->key);
    if (plist->value!=NULL)
        free(plist->value);
    free(plist);
}

static int plist_insert_nocheck(struct plist_s **plist, char *key, char *value)
{
    struct plist_s *p;
    if ( ((p = malloc(sizeof(*p)))==NULL) ||
            ((p->key = strdup(key))==NULL) ||
            ((p->value = strdup(value))==NULL)) {
        plist_destroy_node(p);
        return -1;
    }
    p->next = *plist;
    *plist = p;
    return 0;
}

static int plist_insert_check(struct plist_s **plist, char *key, char *value)
{
    struct plist_s *q;
    for (q=*plist; q!=NULL; q=q->next)
        if (!strcmp(q->key, key)) {
            if (q->value!=NULL) free(q->value);
            q->value = strdup(value);
            return 0;
        }
    return plist_insert_nocheck(plist, key, value);
}

int plist_insert(struct plist_s **plist, char *key, char *value, int checkdup)
{
    if (checkdup)
        return plist_insert_check(plist, key, value);
    return plist_insert_nocheck(plist, key, value);
}

void plist_destroy(struct plist_s **plist)
{
    struct plist_s *p, *q;
    for (q=*plist; q!=NULL; q=p) {
        p = q->next;
        plist_destroy_node(q);
    }
    *plist = NULL;
}

char *plist_search(struct plist_s *plist, char *key)
{
    struct plist_s *p;
    for (p=plist; p!=NULL; p=p->next)
        if (!strcmp(p->key, key))
            return p->value;
    return NULL;
}

