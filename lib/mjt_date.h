/*
    Copyright 2009 Francesco Massei

    This file is part of mojito webserver.

        Mojito is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Mojito is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Mojito.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef H_MJT_DATE_H
#define H_MJT_DATE_H

#include <mjt_types.h>

#if !HAVE_STRFTIME
#   error no strftime defined in the system libraries!
#endif
#if !HAVE_GMTIME
#   error no gmtime defined in the system libraries!
#endif
#if !HAVE_MKTIME
#   error no mktime defined in the system libraries!
#endif

BEGIN_C_DECLS

/* format the current time in some useful formats */
extern char_t* mjt_time_1123_format(timet_t t);
extern char_t* mjt_time_1036_format(timet_t t);
extern char_t *mjt_time_asctime_format(timet_t t);
/* parse the string passed into a date */
extern int_t mjt_parse_date(char_t *date, timet_t *t);

END_C_DECLS

#endif /* H_MJT_DATE_H */
