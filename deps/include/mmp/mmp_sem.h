#ifndef H_MMP_SEM_H
#define H_MMP_SEM_H

#include <stdio.h>
#include <stdlib.h>

/* define a mmp_sem_t type, different between unix and windows */
#ifdef _WIN32
#   include <windows.h>
    typedef HANDLE mmp_sem_t;
#   define MMP_SEM_INVALID  INVALID_HANDLE_VALUE
#else
#   include <semaphore.h>
#   include <fcntl.h>
    typedef sem_t* mmp_sem_t;
#   define MMP_SEM_INVALID  NULL
#endif

#include "mmp_trace.h"

/* open or creates a semaphore */
mmp_sem_t mmp_sem_open(const char *name);
/* close a semaphore */
ret_t mmp_sem_close(mmp_sem_t mutex);
/* wait and lock a semaphore (waiting forever) */
ret_t mmp_sem_waitAndLock(mmp_sem_t mutex);
/* releases a lock on a semaphore */
ret_t mmp_sem_release(mmp_sem_t mutex);

#endif /* H_MMP_SEM_H */
