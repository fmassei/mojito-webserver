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
#ifndef H_MMP_DATE_H
#define H_MMP_DATE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* strptime wrapper */
char *xstrptime(const char *buf, const char *fmt, struct tm *tm);

/* format the current time in some useful formats */
void mmp_time_1123_format(time_t t, char *datestr, size_t strsize);
void mmp_time_1036_format(time_t t, char *datestr, size_t strsize);
void mmp_time_asctime_format(time_t t, char *datestr, size_t strsize);
/* parse the string passed into a date */
int mmp_parse_date(char *date, time_t *t);

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_date_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_DATE_H */
