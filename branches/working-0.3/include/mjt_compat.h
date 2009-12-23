#ifndef H_MJT_COMPAT_H
#define H_MJT_COMPAT_H

/* includes magic */
#if HAVE_CONFIG_H
#   include <config.h>
#endif
#include <stdio.h>
#include <sys/types.h>
#if STDC_HEADERS
#   include <stdlib.h>
#   include <string.h>
#elif HAVE_STRINGS_H
#   include <strings.h>
#endif
#if HAVE_FCNTL_H
#   include <fcntl.h>
#endif
#if HAVE_UNISTD_H
#   include <unistd.h>
#endif
#if HAVE_TIME_H
#   include <time.h>
#endif
#if HAVE_ERRNO_H
#   include <errno.h>
#endif
#if HAVE_LIMITS_H
#   include <limits.h>
#endif
#ifndef errno
extern int errno;
#endif

/* defines magic */
#ifndef EXIT_SUCCESS
#   define EXIT_SUCCESS 0
#   define EXIT_FAILURE 1
#endif
#ifdef __cplusplus
#   define BEGIN_C_DECLS    extern "C" {
#   define END_C_DECLS      }
#else
#   define BEGIN_C_DECLS
#   define END_C_DECLS
#endif

/* functions magic */
#if !HAVE_STRCPY
#   if HAVE_BCOPY
#       define strcpy(dest, src)    bcopy(src, dest, 1+strlen(src))
#   else
#       error no strcpy or bcopy available!
#   endif
#endif
#if !HAVE_BZERO && HAVE_MEMSET
#   define bzero(buf, bytes)    ((void)memset(buf, 0, bytes))
#endif

/* variadic stuff */
#if HAVE_STDARG_H
#   include <stdarg.h>
#   define VA_START(a, f)   va_start(a, f)
#else
#   if HAVE_VARARGS_H
#       include <varargs.h>
#       define VA_START(a, f)   va_start(a)
#   endif
#endif
#ifndef VA_START
#   error no variadic api found!
#endif

#endif /* H_MJT_COMPAT_H */
