#ifndef H_MJT_TYPES_H
#define H_MJT_TYPES_H

#include "mjt_compat.h"
#include "mjt_compilers.h"

/* re-define the standard types */
typedef char char_t;
typedef short short_t;
typedef int int_t;
typedef long long_t;
typedef float float_t;
typedef unsigned char uchar_t;
typedef unsigned short ushort_t;
typedef unsigned int uint_t;
typedef unsigned long ulong_t;

typedef size_t sizet_t;
typedef time_t timet_t;

#if HAVE_SSIZE_T
    typedef ssize_t ssizet_t;
#else
    typedef int_t ssizet_t;
#endif

/* that's useful */
#ifndef NO_NEED_FOR_BOOL_T
    typedef int bool_t;
#   define FALSE   0
#   define TRUE    1
#endif

/* oh-well, why not? */
#ifndef NULL
#   define NULL ((void*)0)
#endif

#endif /* H_MJT_TYPES_H */
