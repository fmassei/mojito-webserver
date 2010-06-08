#ifndef H_MMP_SYNC_H
#define H_MMP_SYNC_H

/* semaphore => semaphore. */
/* mutex => binary semaphore, acquirable/releasable only by the same thread. */
/* event => manually (re)settable binary semaphore. */

#ifdef _WIN32
#   include <windows.h>
    typedef HANDLE mmp_thr_sem_t;
    typedef HANDLE mmp_thr_mtx_t;
#   define MMP_THRSEM_INVALID INVALID_HANDLE_VALUE
#   define MMP_THRMTX_INVALID INVALID_HANDLE_VALUE
#else
#   include <pthread.h>
#   include <semaphore.h>
    typedef sem_t *mmp_thr_sem_t;
    typedef pthread_mutex_t *mmp_thr_mtx_t;
#   define MMP_THRSEM_INVALID NULL
#   define MMP_THRMTX_INVALID NULL
#endif

#include "mmp_trace.h"
#include "mmp_memory.h"

/* semaphore */
mmp_thr_sem_t mmp_thr_sem_create(long initCount, long maxCount);
ret_t mmp_thr_sem_close(mmp_thr_sem_t *sem);
ret_t mmp_thr_sem_lock(mmp_thr_sem_t sem);
ret_t mmp_thr_sem_release(mmp_thr_sem_t sem);

/* mutex */
mmp_thr_mtx_t mmp_thr_mtx_create(void);
ret_t mmp_thr_mtx_close(mmp_thr_mtx_t *mtx);
ret_t mmp_thr_mtx_lock(mmp_thr_mtx_t mtx);
ret_t mmp_thr_mtx_release(mmp_thr_mtx_t mtx);

#endif /* H_MMP_SYNC_H */
