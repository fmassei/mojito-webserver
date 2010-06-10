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
#ifndef H_MMP_TAP_H
#define H_MMP_TAP_H

#include <stdio.h>
#include <stdlib.h>

typedef enum mmp_tap_result_e {
    MMP_TAP_PASSED,
    MMP_TAP_FAILED,
    MMP_TAP_UNTESTED
} t_mmp_tap_result_e;

typedef struct mmp_tap_entry_s {
    t_mmp_tap_result_e result;
    char *desc;
    char *comment;
} t_mmp_tap_entry_s;

typedef struct mmp_tap_cycle_s {
    char *name;
    struct mmp_list_s *tests;
} t_mmp_tap_cycle_s;

#include "mmp_memory.h"
#include "mmp_string.h"
#include "mmp_list.h"
#include "mmp_trace.h"

t_mmp_tap_cycle_s *mmp_tap_startcycle(const char * const name);
ret_t mmp_tap_test(t_mmp_tap_cycle_s *cycle, const char * const desc,
                        const char * const comment, t_mmp_tap_result_e res);
ret_t mmp_tap_test_passed(t_mmp_tap_cycle_s *cycle,
                        const char * const desc, const char * const comment);
ret_t mmp_tap_test_failed(t_mmp_tap_cycle_s *cycle,
                        const char * const desc, const char * const comment);
ret_t mmp_tap_test_skipped(t_mmp_tap_cycle_s *cycle,
                        const char * const desc, const char * const comment);
void mmp_tap_freecycle(t_mmp_tap_cycle_s **cycle);

ret_t mmp_tap_print(t_mmp_tap_cycle_s *cycle, FILE *out);

#endif /* H_MMP_TAP_H */
