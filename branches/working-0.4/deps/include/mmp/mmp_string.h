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
#ifndef H_MMP_STRING_H
#define H_MMP_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmp_error.h"
#include "mmp_memory.h"

/* unix strdup() */
char *xstrdup(const char *s);
/* unix index() */
char *xindex(char *s, char c);

/* trim functions */
int mmp_str_is_trimmable(char c);   /* is a character "trimmable"? */
char *mmp_str_ltrim(char *str);     /* left trim */
char *mmp_str_rtrim(char *str);     /* right trim */
char *mmp_str_trim(char *str);      /* left and right trim */

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_string_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_STRING_H */
