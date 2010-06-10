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
#ifndef H_MMP_SHM_H
#define H_MMP_SHM_H

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#   include <windows.h>
#   include <winnt.h>
#   include <memory.h>
    typedef HANDLE t_mmp_shm_mapf;
#else
#   include <sys/types.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <unistd.h>
#   include <fcntl.h>
    typedef int t_mmp_shm_mapf;
#endif
#include "mmp_memory.h"
#include "mmp_string.h"
#include "mmp_trace.h"

typedef struct mmp_shm_s {
    void *base;
    size_t len;
    char *filename;
    t_mmp_shm_mapf map;
} t_mmp_shm_s;

t_mmp_shm_s *mmp_shm_attach(const char *name, size_t size);
ret_t mmp_shm_getdata(t_mmp_shm_s *shm, void *dst, size_t from, size_t len);
ret_t mmp_shm_putdata(t_mmp_shm_s *shm, void *src, size_t from, size_t len);
void mmp_shm_detach(t_mmp_shm_s **map);

#endif /* H_MMP_SHM_H */
