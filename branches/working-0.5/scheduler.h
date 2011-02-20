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
#ifndef H_SCHEDULER_H
#define H_SCHEDULER_H

#include <stdlib.h>
#include <mmp/mmp_error.h>
#include <mmp/mmp_socket.h>

typedef int t_sched_id;

t_sched_id scheduler_create(size_t pool_size);
void scheduler_destroy(t_sched_id sched_id);
ret_t scheduler_add_listen_socket(t_sched_id sched_id, t_socket sock);
ret_t scheduler_add_client_socket(t_sched_id sched_id, t_socket sock);
ret_t scheduler_del_socket(t_sched_id sched_id, t_socket sock);
ret_t scheduler_loop(t_sched_id sched_id, void(*cback_fp)(t_socket));

#endif /* H_SCHEDULER_H */

