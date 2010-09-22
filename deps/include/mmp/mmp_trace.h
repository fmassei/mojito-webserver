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
/** \file   mmp_trace.h
 * \brief   error tracing functions
 * \author  FtM
 * \date    2010-Jul-08
 *
 * this module is completely statically allocated to keep working in case of
 * memory exaustion (one of the cases in which we'd like to trace where things
 * broke ;) )
 */
#ifndef H_MMP_TRACE_H
#define H_MMP_TRACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmp_compat.h"
#include "mmp_error.h"

/** \brief low-level trace creator. Use the mmp_setError() macros instead!
 * \internal */
void mmp_trace_create(const char * __restrict f, int l, ret_t ret, err_t err,
                      const char * __restrict extramsg);

/** \brief set an error on the trace list */
#define mmp_setError(_R)    \
    mmp_trace_create(__FILE__, __LINE__, (_R), errno, NULL)
/** \brief set an error on the trace list, with an extra message */
#define mmp_setError_ext(_R, _E)    \
    mmp_trace_create(__FILE__, __LINE__, (_R), errno, (_E))

/** \brief print out the complete trace list messages */
void mmp_trace_print(FILE *out);

/** \brief reset the trace stack */
void mmp_trace_reset(void);

/** \brief assert()-like macro for parameter check */
#define MMP_CHECK_OR_RETURN(_COND, _RET) \
    if (!(_COND)) { \
        mmp_setError(MMP_ERR_PARAMS); \
        return (_RET); \
    }

#endif /* H_MMP_TRACE_H */
