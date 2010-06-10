/*
    Copyright 2010 Francesco Massei

    This file is part of the MMP library.

        MMP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MMP.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef H_MMP_HTAB_H
#define H_MMP_HTAB_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mmp_memory.h"
#include "mmp_trace.h"
#include "mmp_string.h"

typedef struct mmp_htab_elem_s {
    char *key;
    void *val;
    struct mmp_htab_elem_s *next, *prev;
} t_mmp_htab_elem_s;

typedef struct mmp_htab_s {
    size_t size;
    t_mmp_htab_elem_s **ptrs;
} t_mmp_htab_s;

t_mmp_htab_s *mmp_htab_create(size_t size);
void mmp_htab_destroy(t_mmp_htab_s **ptr);
void mmp_htab_destroy_with_data(t_mmp_htab_s **ptr, void(*datadel)(void**));
unsigned int mmp_htab_hash(t_mmp_htab_s *htab, char *str, int max);
void *mmp_htab_lookup_nz(t_mmp_htab_s *htab, char *key, int keylen);
void *mmp_htab_lookup(t_mmp_htab_s *htab, char *key);
ret_t mmp_htab_install(t_mmp_htab_s *htab, char *key, void *val);
ret_t mmp_htab_delete(t_mmp_htab_s *htab, char *key);

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_htab_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_HTAB_H */
