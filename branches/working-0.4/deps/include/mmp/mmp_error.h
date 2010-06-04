#ifndef H_MMP_ERROR_H
#define H_MMP_ERROR_H

/* some error functions wraps */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mmp_compat.h"

/* internal type error */
enum error_e {
    MMP_ERR_OK,
    MMP_ERR_GENERIC,
    MMP_ERR_PARAMS,
    MMP_ERR_ENOMEM,
    MMP_ERR_FILE,
    MMP_ERR_DL,
    MMP_ERR_PARSE,
    MMP_ERR_SEMAPHORE,
    MMP_ERR_SHM,
    MMP_ERR_SOCKET,
    MMP_ERR_SYNC
};
typedef enum error_e ret_t;     /* mmp error */
typedef int err_t;              /* system error */

/* get internal error description */
const char * const mmp_error_ret_getdesc(ret_t ret);
/* get system error description */
const char * const mmp_error_err_getdesc(err_t err);

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_error_unittest(struct mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_ERROR_H */
