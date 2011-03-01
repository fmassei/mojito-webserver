/*
    Copyright 2011 Francesco Massei

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
#include "scheduler_fake.h"
#ifdef BUILD_FAKE_SCHEDULER

t_sched_id scheduler_fake_create(size_t pool_size)
{
    mmp_setError_ext(MMP_ERR_GENERIC, "Could not work with fake scheduler!");
    return -1;
}

void scheduler_fake_destroy(t_sched_id sched_id)
{
}

ret_t scheduler_fake_add_listen_socket(t_sched_id sched_id, t_socket sock)
{
    mmp_setError_ext(MMP_ERR_GENERIC, "Could not work with fake scheduler!");
    return MMP_ERR_GENERIC;
}

ret_t scheduler_fake_add_client_socket(t_sched_id sched_id, t_socket sock)
{
    mmp_setError_ext(MMP_ERR_GENERIC, "Could not work with fake scheduler!");
    return MMP_ERR_GENERIC;
}

ret_t scheduler_fake_del_socket(t_sched_id sched_id, t_socket sock)
{
    mmp_setError_ext(MMP_ERR_GENERIC, "Could not work with fake scheduler!");
    return MMP_ERR_GENERIC;
}

t_sched_ret_e scheduler_fake_loop(t_sched_id sched_id, t_schedfnc_fp cback_fp)
{
    mmp_setError_ext(MMP_ERR_GENERIC, "Could not work with fake scheduler!");
    return SCHEDRET_ERR;
}

#endif /* BUILD_FAKE_SCHEDULER */

