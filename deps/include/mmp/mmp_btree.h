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
/** \file   mmp_btree.h
 * \brief   B-tree datatypes and functions
 * \author  FtM
 * \date    2010-Sep-02
 */
#ifndef H_MMP_BTREE_H
#define H_MMP_BTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_trace.h"
#include "mmp_files.h"
#include "mmp_mmap.h"

/** \brief btree key type */
typedef int t_mmp_btkey;
/** \brief btree value type */
typedef int t_mmp_btval;

/** \brief btree struct */
typedef struct mmp_bt_s t_mmp_bt_s;

/** \brief create or open a btree */
t_mmp_bt_s *mmp_bt_create(const char *fname, int page_size);
/** \brief search the btree for a key */
ret_t mmp_bt_search(t_mmp_bt_s *bt, t_mmp_btkey key, t_mmp_btval *val);
/** \brief insert a key/val into the btree */
ret_t mmp_bt_insert(t_mmp_bt_s *bt, t_mmp_btkey key, t_mmp_btval val);
/** \brief destroy the btree */
void mmp_bt_destroy(t_mmp_bt_s **bt);

#ifdef UNIT_TESTING
#include "mmp_tap.h"
#include "mmp_system.h"
ret_t mmp_btree_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_BTREE_H */
