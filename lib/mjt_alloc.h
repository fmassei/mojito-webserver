#ifndef H_MJT_ALLOC_H
#define H_MJT_ALLOC_H

#include <mjt_types.h>

#define mjt_free(PPTR) do { \
    if ((*PPTR)!=NULL) { free(*PPTR); } } while(0)
#define mjt_free2null(PPTR) do { \
    mjt_free(PPTR); (*PPTR) = NULL; } while(0)

BEGIN_C_DECLS

extern void *mjt_malloc(sizet_t num);
extern void *mjt_calloc(sizet_t num, sizet_t size);
extern void *mjt_realloc(void *ptr, sizet_t num);

END_C_DECLS

#endif /* H_MJT_ALLOC_H */
