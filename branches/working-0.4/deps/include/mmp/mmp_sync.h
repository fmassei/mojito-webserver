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
#ifndef H_MMP_SYNC_H
#define H_MMP_SYNC_H

/* semaphore => semaphore. */
/* mutex => binary semaphore, acquirable/releasable only by the same thread. */
/* event => manually (re)settable binary semaphore. */

#ifdef _WIN32
#   include <windows.h>
    typedef HANDLE t_mmp_thr_sem;
    typedef HANDLE t_mmp_thr_mtx;
    typedef HANDLE t_mmp_thr_evt;
#   define MMP_THRSEM_INVALID INVALID_HANDLE_VALUE
#   define MMP_THRMTX_INVALID INVALID_HANDLE_VALUE
#   define MMP_THREVT_INVALID INVALID_HANDLE_VALUE
#else
#   include <pthread.h>
#   include <semaphore.h>
    typedef sem_t *t_mmp_thr_sem;
    typedef pthread_mutex_t *t_mmp_thr_mtx;
    typedef sem_t *t_mmp_thr_evt;
#   define MMP_THRSEM_INVALID NULL
#   define MMP_THRMTX_INVALID NULL
#   define MMP_THREVT_INVALID NULL
#endif

#include "mmp_trace.h"
#include "mmp_memory.h"

/* semaphore */
t_mmp_thr_sem mmp_thr_sem_create(long initCount, long maxCount);
ret_t mmp_thr_sem_close(t_mmp_thr_sem *sem);
ret_t mmp_thr_sem_lock(t_mmp_thr_sem sem);
ret_t mmp_thr_sem_release(t_mmp_thr_sem sem);

/* mutex */
t_mmp_thr_mtx mmp_thr_mtx_create(void);
ret_t mmp_thr_mtx_close(t_mmp_thr_mtx *mtx);
ret_t mmp_thr_mtx_lock(t_mmp_thr_mtx mtx);
ret_t mmp_thr_mtx_release(t_mmp_thr_mtx mtx);

/* event */
t_mmp_thr_evt mmp_thr_evt_create();
ret_t mmp_thr_evt_close(t_mmp_thr_evt *evt);
ret_t mmp_thr_evt_signal(t_mmp_thr_evt evt);
ret_t mmp_thr_evt_wait(t_mmp_thr_evt evt);

#endif /* H_MMP_SYNC_H */
