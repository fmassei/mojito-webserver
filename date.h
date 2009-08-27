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

#ifndef H_DATE_H
#define H_DATE_H

#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* format the current time in some useful formats */
char* time_1123_format(time_t t);
char* time_1036_format(time_t t);
char *time_asctime_format(time_t t);
/* parse the string passed into a date */
int parse_date(char *date, time_t *t);

#endif /* H_DATE_H */
