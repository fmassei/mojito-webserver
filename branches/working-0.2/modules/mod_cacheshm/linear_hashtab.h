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

#ifndef H_LINEAR_HASHTAB_H
#define H_LINEAR_HASHTAB_H

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../../logger.h"

/* This module is an hack. It implements one of my favourite type of
 * algorithms: the linear cached versions of the simple ones ;-) This one is a
 * common hash table, but stored within a memory mapped file for multi-process
 * access. */

struct lh_s {
    unsigned char *addr;
    unsigned int size, step;
};

struct entry_s {
    unsigned int len : 31;
    unsigned int b : 1;     /* busy flag */
    void *key;              /* theese are not pointers, but variable arrays */
    void *data;
};

/* standard hashtab operations: create/destroy/lookup/install */
struct lh_s *lhcreate(unsigned int size);
void lhdestroy(struct lh_s *lh);
struct entry_s *lhlookup(struct lh_s *lh, char *key);
int lhinstall(struct lh_s *lh, char *key, void *data,unsigned int dlen);

#endif /* H_LINEAR_HASHTAB_H */
