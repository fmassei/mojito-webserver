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

/** \file   mmp_compat.h
 *  \brief  compability features
 *  \author FtM
 *  \date   2010-Jul-8
 */
#ifndef H_MMP_COMPAT_H
#define H_MMP_COMPAT_H

/* restrict features */
/** \def __inline
 * \brief compiler inline directive, activated if possible */
/** \def __restrict
 * \brief compiler restrict directive, activated if possible */
#if __GNUC__ > 2
#   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901
#       define __restrict
#       define __inline
#   else
#       define __restrict   restrict
#       define __inline     inline
#   endif
#endif

/** \def xstrncpy()
 * \brief unix strncpy */
#ifdef _WIN32
#   define xstrncpy(_S1, _S2, _N)   strcpy_s((_S1), (_N), (_S2))
#else
#   define xstrncpy(_S1, _S2, _N)   strncpy((_S1), (_S2), (_N))
#endif

/** \def xstrerror() 
 * \brief unix strerror */
#define xstrerror(_E)   strerror(_E)

/* some types on win */
#ifndef _WIN32
#include <stdint.h>
#else
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
#endif

#endif /* H_MMP_COMPAT_H */
