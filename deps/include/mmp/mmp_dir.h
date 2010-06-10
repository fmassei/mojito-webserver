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
#ifndef H_MMP_DIR_H
#define H_MMP_DIR_H

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#   include <sys/types.h>
#   include <dirent.h>
#else
#   include <windows.h>
#endif

#include "mmp_list.h"
#include "mmp_trace.h"
#include "mmp_memory.h"
#include "mmp_string.h"

/* list all files in a directory */
t_mmp_list_s *mmp_dir_listfiles(const char * const dirname);

/* free the results of a mmp_dir_listfiles */
void mmp_dir_free_list_results(t_mmp_list_s **list);

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_dir_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_DIR_H */
