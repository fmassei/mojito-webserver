#ifndef H_MJT_COMPILERS_H
#define H_MJT_COMPILERS_H

#ifndef INLINE
#   if __GNUC__ && !__GNUC_STDC_INLINE__
#       define INLINE extern inline
#   else
#       define INLINE __inline__
#   endif
#endif

#ifndef __GNUC__
#   define typeof(type) void*
#endif

/* to stop the compiler complaints */
#define UNUSED(arg) arg=arg

#endif /* H_MJT_COMPILERS_H */
