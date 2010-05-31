#ifndef H_MMP_THREAD_H
#define H_MMP_THREAD_H

#ifndef _WIN32
#   include <pthread.h>
    typedef pthread_t mmp_thread_t;
#else
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   include <windows.h>
    typedef HANDLE mmp_thread_t;
#endif

#include "mmp_memory.h"
#include "mmp_trace.h"

typedef void*(*mmp_thread_start_t)(void *);
ret_t mmp_thread_create(mmp_thread_start_t fnc, void *arg, mmp_thread_t *out);
ret_t mmp_thread_join(mmp_thread_t *id);
void mmp_thread_exit(int code);

#endif /* H_MMP_THREAD_H */
