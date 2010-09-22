/*
    Copyright 2010 Francesco Massei

    This file is part of the MMP library.

        MMP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MMP.  If not, see <http://www.gnu.org/licenses/>.
*/
/** \file   mmp_memory.h
 * \brief   malloc/calloc/realloc/free wrappers
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_MEMORY_H
#define H_MMP_MEMORY_H

#include <stdio.h>
#include <stdlib.h>

/** \brief malloc wrapper */
#define xmalloc(_S)         malloc((_S))
/** \brief callc wrapper */
#define xcalloc(_N, _S)     calloc((_N), (_S))
/** \brief free wrapper */
#define xfree(_P)           free((_P))
/** \brief realloc wrapper */
#define xrealloc(_P, _S)    realloc((_P), (_S))

/* utility macros */
/** \brief try to malloc, set ENOMEM and return _RET on failure */
#define MMP_XMALLOC_OR_RETURN(_PTR, _RET)  \
    if (((_PTR) = xmalloc(sizeof(*(_PTR))))==NULL) { \
        mmp_setError(MMP_ERR_ENOMEM); \
        return (_RET); \
    }
/** \brief free _PTR and set it to NULL */
#define MMP_XFREE_AND_NULL(_PTR)    \
    xfree(_PTR); \
    (_PTR) = NULL;

#endif /* H_MMP_MEMORY_H */
