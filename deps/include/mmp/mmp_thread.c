#include "mmp_thread.h"

ret_t mmp_thread_create(mmp_thread_start_t fnc, void *arg, mmp_thread_t *out)
{
#ifndef _WIN32
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    pthread_attr_setstacksize(&threadAttr, 4*1024);
    pthread_create(out, &threadAttr, fnc, arg);
    pthread_attr_destroy(&threadAttr);
#else
    DWORD id;
    *out = CreateThread(NULL, 4*1024, (LPTHREAD_START_ROUTINE)fnc, arg, 0, &id);
#endif
    return MMP_ERR_OK;
}

ret_t mmp_thread_join(mmp_thread_t *id)
{
#ifndef _WIN32
    pthread_join(id, NULL);
#else
    WaitForSingleObject(*id, INFINITE);
#endif
    return MMP_ERR_OK;
}

void mmp_thread_exit(int code)
{
#ifndef _WIN32
    pthread_exit(NULL);
#else
    ExitThread((DWORD)code);
#endif
}

#ifdef UNIT_TESTING
int glob;
static void* thtest(void* ptr)
{
    ++glob;
    mmp_thread_exit(0);
}
static enum mmp_tap_result_e test_threads(void)
{
    int i;
    mmp_thread_t id[10];
    glob = 0;
    for (i=0; i<10; ++i) {
        if (mmp_thread_create(thtest, NULL, &(id[i]))!=MMP_ERR_OK)
            return MMP_TAP_FAILED;
    }
    for (i=0; i<10; ++i)
        if (mmp_thread_join(&(id[i]))!=MMP_ERR_OK)
            return MMP_TAP_FAILED;
    if (glob!=10)
        return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}
ret_t mmp_thread_unittest(struct mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if (
            ((ret=mmp_tap_test(cycle, "mmp_thread funcs", NULL,
                                        test_threads()))!=MMP_ERR_OK)
        )
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */
