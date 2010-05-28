#ifndef H_MMP_MEMORY_H
#define H_MMP_MEMORY_H

#include <stdio.h>
#include <stdlib.h>

#define xmalloc(_S)         malloc((_S))
#define xcalloc(_N, _S)     calloc((_N), (_S))
#define xfree(_P)           free((_P))
#define xrealloc(_P, _S)    realloc((_P), (_S))

#endif /* H_MMP_MEMORY_H */
