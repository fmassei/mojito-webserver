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
/** \file   mmp_mmap.h
 * \brief   mmap wrapper
 * \author  FtM
 * \date    2010-Aug-24
 */
#ifndef H_MMP_MMAP_H
#define H_MMP_MMAP_H

#include <stdio.h>
#ifndef _WIN32
#   include <unistd.h>
#   include <sys/mman.h>
#   include <limits.h>
#else
#   include <windows.h>
#   include <io.h>
#   define MAP_FAILED NULL
/*  some defines that are not used anyway... */
/** \todo define this ones.. */
#   define PROT_READ    PAGE_READONLY
#   define PROT_WRITE   PAGE_READWRITE
#   define MAP_PRIVATE  0
#   define MAP_SHARED   0
#endif
#include "mmp_h_utils.h"
#include "mmp_system.h"
#include "mmp_memory.h"
#include "mmp_trace.h"

typedef struct mmp_mmap_s {
    void *ptr;
    size_t length;
#ifdef _WIN32
    HANDLE hmap;
#endif
} t_mmp_mmap_s;

t_mmp_mmap_s *mmp_mmap(void *ptr, size_t size, int prot, int flags, int fd,
                                                                long offset);
int mmp_munmap(t_mmp_mmap_s** map);

#endif /* H_MMP_MMAP_H */
