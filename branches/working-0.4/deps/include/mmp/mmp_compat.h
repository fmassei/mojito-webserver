#ifndef H_MMP_COMPAT_H
#define H_MMP_COMPAT_H

/* unix strncpy() */
#ifdef _WIN32
#   define xstrncpy(_S1, _S2, _N)   strcpy_s((_S1), (_N), (_S2))
#else
#   define xstrncpy(_S1, _S2, _N)   strncpy((_S1), (_S2), (_N))
#endif

/* unix strerror() */
#define xstrerror(_E)   strerror(_E)

#endif /* H_MMP_COMPAT_H */
