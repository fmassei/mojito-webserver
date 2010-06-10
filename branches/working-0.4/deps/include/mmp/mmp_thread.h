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
#ifndef H_MMP_THREAD_H
#define H_MMP_THREAD_H

#ifndef _WIN32
#   include <pthread.h>
    typedef pthread_t t_mmp_thread;
#else
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   include <windows.h>
    typedef HANDLE t_mmp_thread;
#endif

#include "mmp_memory.h"
#include "mmp_trace.h"

typedef void*(*t_mmp_thread_start)(void *);
ret_t mmp_thread_create(t_mmp_thread_start fnc, void *arg, t_mmp_thread *out);
ret_t mmp_thread_join(t_mmp_thread *id);
void mmp_thread_exit(int code);

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_thread_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_THREAD_H */
