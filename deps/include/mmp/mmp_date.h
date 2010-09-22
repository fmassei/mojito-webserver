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

/** \file   mmp_date.h
 * \brief   common date management functions
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_DATE_H
#define H_MMP_DATE_H

#ifndef _WIN32
#   define _XOPEN_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mmp_compat.h"
#include "mmp_trace.h"

/** unix strptime wrapper
 * \todo missing unittest */
char *xstrptime(const char *buf, const char *fmt, struct tm *tm);

/** format the current time in RFC-1123 format 
 * (Sun, 06 Nov 1994 08:49:37 GMT) */
void mmp_time_1123_format(time_t t, char * datestr, size_t strsize);

/** format the current time in RFC-1036 format
 * (Sunday, 06-Nov-94 08:49:37 GMT) */
void mmp_time_1036_format(time_t t, char * datestr, size_t strsize);

/** format the current time in asctime format */
void mmp_time_asctime_format(time_t t, char * datestr, size_t strsize);

/** \brief parse the passed string into a date
 * try all the parsing between 1123, 1036 and asctime
 * \return MMP_ERR_PARSE if no available parsing where possible, MMP_ERR_OK
 *          otherwise. */
ret_t mmp_parse_date(const char * __restrict date,time_t * __restrict t);

/** \brief get timezone */
char *mmp_get_tm_zone(void);
/** \brief set timezone */
void mmp_set_tm_zone(char *zone);

#ifdef UNIT_TESTING
#include "mmp_tap.h"
/** date unit testing */
ret_t mmp_date_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_DATE_H */
