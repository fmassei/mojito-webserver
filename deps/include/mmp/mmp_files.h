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
/** \file   mmp_files.h
 * \brief   common file management functions
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_FILES_H
#define H_MMP_FILES_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>

#ifndef _WIN32
#   include <unistd.h>
#   ifndef XOPEN_SOURCE
#       define XOPEN_SOURCE 500
#   endif
#   ifndef __USE_XOPEN
#       define __USE_XOPEN
#   endif
#else
#   include <windows.h>
#   include <io.h>
#   include <share.h>
#endif
#include <sys/stat.h>

#include "mmp_compat.h"
#include "mmp_h_utils.h"
#include "mmp_list.h"
#include "mmp_trace.h"
#include "mmp_memory.h"
#include "mmp_string.h"

/** unix stat structure */
#ifndef _WIN32
    typedef struct stat t_mmp_stat_s;
#else
    typedef struct _stat t_mmp_stat_s;
#endif

/** unix open */
int mmp_open(const char *path, int flags, int mode);
/** unix read */
int mmp_read(int fd, void *buf, size_t count);
/** unix write */
int mmp_write(int fd, const void *buf, size_t count);
/** unix close */
int mmp_close(int fd);

/** unix pwrite */
int mmp_pwrite(int fd, const void *buf, size_t nbyte, long offset);

/** unix stat */
ret_t mmp_stat(const char *__restrict path, t_mmp_stat_s *__restrict stat_ptr);
/** unix fstat */
ret_t mmp_fstat(int fd, t_mmp_stat_s * stat_ptr);

#ifdef UNIT_TESTING
#   include "mmp_tap.h"
    ret_t mmp_files_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_FILES_H */
