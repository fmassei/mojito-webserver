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
