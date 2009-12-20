#ifndef H_MJT_TYPES_H
#define H_MJT_TYPES_H

#include "mjt_compat.h"
#include "mjt_compilers.h"

typedef char char_t;
typedef short short_t;
typedef int int_t;
typedef long long_t;

typedef unsigned char uchar_t;
typedef unsigned short ushort_t;
typedef unsigned int uint_t;
typedef unsigned long ulong_t;

typedef size_t sizet_t;
typedef time_t timet_t;

#ifndef NO_NEED_FOR_BOOL_T
    typedef int bool_t;
#   define FALSE   0
#   define TRUE    1
#endif

#ifndef NULL
#   define NULL ((void*)0)
#endif

#endif /* H_MJT_TYPES_H */
