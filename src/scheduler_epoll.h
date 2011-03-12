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
#ifndef H_SCHEDULER_EPOLL_H
#define H_SCHEDULER_EPOLL_H
#include "scheduler.h"

#ifdef BUILD_EPOLL_SCHEDULER
t_sched_id scheduler_epoll_create(size_t pool_size);
void scheduler_epoll_destroy(t_sched_id sched_id);
ret_t scheduler_epoll_add_listen_socket(t_sched_id sched_id, t_socket sock);
ret_t scheduler_epoll_add_client_socket(t_sched_id sched_id, t_socket sock);
ret_t scheduler_epoll_del_socket(t_sched_id sched_id, t_socket sock);
t_sched_ret_e scheduler_epoll_loop(t_sched_id sched_id, t_schedfnc_fp cback_fp,
                                        int millisecs, t_schedto_fp cback_to);
#endif /* BUILD_EPOLL_SCHEDULER */

#endif /* H_SCHEDULER_EPOLL_H */
