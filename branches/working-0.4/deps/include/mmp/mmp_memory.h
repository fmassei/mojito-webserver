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
#ifndef H_MMP_MEMORY_H
#define H_MMP_MEMORY_H

#include <stdio.h>
#include <stdlib.h>

#define xmalloc(_S)         malloc((_S))
#define xcalloc(_N, _S)     calloc((_N), (_S))
#define xfree(_P)           free((_P))
#define xrealloc(_P, _S)    realloc((_P), (_S))

#endif /* H_MMP_MEMORY_H */
