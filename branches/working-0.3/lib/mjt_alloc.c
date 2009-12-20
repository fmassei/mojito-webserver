#include "mjt_alloc.h"

BEGIN_C_DECLS

void *mjt_malloc(sizet_t num)
{
    return malloc(num);
}

void *mjt_calloc(sizet_t num, sizet_t size)
{
    void *newp;
    newp = mjt_malloc(num * size);
    bzero(newp, num*size);
    return newp;
}

void *mjt_realloc(void *ptr, sizet_t num)
{
    if (!ptr)
        return mjt_malloc(num);
    return realloc(ptr, num);
}

END_C_DECLS

