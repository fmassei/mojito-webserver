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

#include "mjt_date.h"

static char_t datestr[40];

/* try to parse a RFC-1123 (old 822) format (Sun, 06 Nov 1994 08:49:37 GMT) */
static int_t parse_1123_date(char_t *date, timet_t *t)
{
    char_t *c;
    struct tm tt;
    c = strptime(date, "%a, %d %b %Y %H:%M:%S GMT", &tt);
    if ((c==NULL)||(*c!='\0'))
        return -1;
    *t = mktime(&tt);
    return 0;
}

/* try to parse a RFC-1036 (old 822) format (Sunday, 06-Nov-94 08:49:37 GMT) */
static int_t parse_1036_date(char_t *date, timet_t *t)
{
    char_t *c;
    struct tm tt;
    c = strptime(date, "%A, %d-%b-%y %H:%M:%S GMT", &tt);
    if ((c==NULL)||(*c!='\0'))
        return -1;
    *t = mktime(&tt);
    return 0;
}

static int_t parse_asctime_date(char_t *date, timet_t *t)
{
    char_t *c;
    struct tm tt;
    c = strptime(date, "%a %b %d %H:%M:%S %Y", &tt);
    if ((c==NULL)||(*c!='\0'))
        return -1;
    *t = mktime(&tt);
    return 0;
}

BEGIN_C_DECLS

/* format in a RFC-1123 (old 822) format (Sun, 06 Nov 1994 08:49:37 GMT) */
char_t* mjt_time_1123_format(timet_t t)
{
    strftime(datestr, sizeof(datestr), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
    return datestr;
}

/* format in a RFC-1036 (old 850) format (Sunday, 06-Nov-94 08:49:37 GMT) */
char_t* mjt_time_1036_format(timet_t t)
{
    strftime(datestr, sizeof(datestr), "%A, %d-%b-%y %H:%M:%S GMT", gmtime(&t));
    return datestr;
}

/* format in asctime format */
char_t *mjt_time_asctime_format(timet_t t)
{
    strftime(datestr, sizeof(datestr), "%a %b %d %H:%M:%S %Y", gmtime(&t));
    return datestr;
}

/* try all the parsing, returns -1 if no available parsing where possible */
int_t mjt_parse_date(char_t *date, timet_t *t)
{
    if (parse_1123_date(date, t)==0)
        return 0;
    if (parse_1036_date(date, t)==0)
        return 0;
    if (parse_asctime_date(date, t)==0)
        return 0;
    return -1;
}

END_C_DECLS

