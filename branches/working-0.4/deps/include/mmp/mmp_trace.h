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
#ifndef H_MMP_TRACE_H
#define H_MMP_TRACE_H

/* generic tracing module. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmp_compat.h"
#include "mmp_error.h"

/* low-level trace creator. Use the mmp_setError() macros instead! */
void mmp_trace_create(const char * const f, int l, ret_t ret, err_t err,
                      const char * const extramsg);

/* set an error on the trace list */
#define mmp_setError(_R)    \
    mmp_trace_create(__FILE__, __LINE__, (_R), errno, NULL)
/* set an error on the trace list, with an extra message */
#define mmp_setError_ext(_R, _E)    \
    mmp_trace_create(__FILE__, __LINE__, (_R), errno, (_E))

/* print out the complete trace list messages */
void mmp_trace_print(FILE *out);

/* reset the trace stack */
void mmp_trace_reset(void);

#endif /* H_MMP_TRACE_H */
