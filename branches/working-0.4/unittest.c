/*
    Copyright 2010 Francesco Massei

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
#ifdef UNIT_TESTING
#include <stdio.h>
#include <stdlib.h>
#include "header_w_quality.h"
#include <mmp/mmp_tap.h>

int main(void)
{
    t_mmp_tap_cycle_s *cycle;
    if ((cycle = mmp_tap_startcycle("mojito unit test"))==NULL) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    if (    (test_qhead_unittest(cycle)!=MMP_ERR_OK) ||
            (mmp_tap_print(cycle, stdout)!=MMP_ERR_OK) ) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    mmp_tap_freecycle(&cycle);
#ifdef _WIN32
    system("PAUSE");
#endif
    return 0;
}
#else
static void stripme(void) {;}
#endif /* UNIT_TESTING */
