#ifndef H_MMP_SHM_H
#define H_MMP_SHM_H

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#   include <windows.h>
#   include <winnt.h>
#   include <memory.h>
    typedef HANDLE mmp_shm_mapf_t;
#else
#   include <sys/types.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <unistd.h>
#   include <fcntl.h>
    typedef int mmp_shm_mapf_t;
#endif
#include "mmp_memory.h"
#include "mmp_string.h"
#include "mmp_sem.h"

typedef struct mmp_shm_s {
    void *base;
    size_t len;
    char *filename;
    mmp_shm_mapf_t map;
} mmp_shm_st;

mmp_shm_st *mmp_shm_attach(const char *name, size_t size);
ret_t mmp_shm_getdata(mmp_shm_st *shm, void *dst, size_t from, size_t len);
ret_t mmp_shm_putdata(mmp_shm_st *shm, void *src, size_t from, size_t len);
void mmp_shm_detach(mmp_shm_st **map);

#endif /* H_MMP_SHM_H */
