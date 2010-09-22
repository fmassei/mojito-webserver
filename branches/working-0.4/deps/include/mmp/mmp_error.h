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
#ifndef H_MMP_ERROR_H
#define H_MMP_ERROR_H

/** \file   mmp_error.h
 * \brief   some error functions wraps
 * \author  FtM
 * \date    2010-Jul-08
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mmp_compat.h"

/** internal type error */
typedef enum mmp_error_e {
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
    MMP_ERR_SYNC,
    MMP_ERR_FULL,
    MMP_ERR_EMPTY,
    MMP_ERR_NOTFOUND
} t_mmp_error_e;

/** mmp internal error type */
typedef t_mmp_error_e ret_t;    /* mmp error */

/** system error type */
typedef int err_t;              /* system error */

/** get internal error description */
const char * const mmp_error_ret_getdesc(ret_t ret);
/** get system error description */
const char * const mmp_error_err_getdesc(err_t err);

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_error_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_ERROR_H */
