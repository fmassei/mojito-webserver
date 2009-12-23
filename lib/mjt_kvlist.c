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

#include "mjt_kvlist.h"
#include "mjt_alloc.h"

static void kvlist_destroy_node(struct kvlist_s **ptr)
{
    if (ptr==NULL || *ptr==NULL) return;
    mjt_free2null((*ptr)->key);
    mjt_free2null((*ptr)->val);
    mjt_free2null(ptr);
}

static int_t kvlist_insert_nocheck(struct kvlist_s **ptr, char_t *k, char_t *v)
{
    struct kvlist_s *p;
    if ( ((p = mjt_malloc(sizeof(*p)))==NULL) ||
            ((p->key = strdup(k))==NULL) ||
            ((p->val = strdup(v))==NULL)) {
        kvlist_destroy_node(&p);
        return -1;
    }
    p->next = *ptr;
    *ptr = p;
    return 0;
}

static int_t kvlist_insert_check(struct kvlist_s **ptr, char_t *k, char_t *v)
{
    struct kvlist_s *q;
    for (q=*ptr; q!=NULL; q=q->next)
        if (!strcmp(q->key, k)) {
            if (q->val!=NULL) mjt_free2null(q->val);
            q->val = strdup(v);
            return 0;
        }
    return kvlist_insert_nocheck(ptr, k, v);
}

int_t mjt_kvlist_insert(struct kvlist_s **ptr, char_t *key, char_t *value,
                    int_t checkdup)
{
    if (checkdup)
        return kvlist_insert_check(ptr, key, value);
    return kvlist_insert_nocheck(ptr, key, value);
}

void mjt_kvlist_destroy(struct kvlist_s **ptr)
{
    struct kvlist_s *p, *q;
    for (q=*ptr; q!=NULL; q=p) {
        p = q->next;
        kvlist_destroy_node(&q);
    }
    *ptr = NULL;
}

char_t *mjt_kvlist_search(struct kvlist_s *ptr, char_t *key)
{
    struct kvlist_s *p;
    for (p=ptr; p!=NULL; p=p->next)
        if (!strcmp(p->key, key))
            return p->val;
    return NULL;
}

