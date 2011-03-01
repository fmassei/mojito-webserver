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
#include "utils.h"

typedef int t_sched_id;

typedef enum sched_ret_e {
    SCHEDRET_OK,
    SCHEDRET_ERR,
    SCHEDRET_CBACKERR,
} t_sched_ret_e;
typedef enum schedfnc_ret_e {
    SCHEDFNCRET_OK,
    SCHEDFNCRET_ERR
} t_schedfnc_ret_e;
typedef t_schedfnc_ret_e(*t_schedfnc_fp)(t_socket);

#ifdef HAVE_SYS_EPOLL_H
#   define BUILD_EPOLL_SCHEDULER
#   include "scheduler_epoll.h"
#   define scheduler_create             scheduler_epoll_create
#   define scheduler_destroy            scheduler_epoll_destroy
#   define scheduler_add_listen_socket  scheduler_epoll_add_listen_socket
#   define scheduler_add_client_socket  scheduler_epoll_add_client_socket
#   define scheduler_del_socket         scheduler_epoll_del_socket
#   define scheduler_loop               scheduler_epoll_loop
#else
#   define BUILD_FAKE_SCHEDULER
#   include "scheduler_fake.h"
#   define scheduler_create             scheduler_fake_create
#   define scheduler_destroy            scheduler_fake_destroy
#   define scheduler_add_listen_socket  scheduler_fake_add_listen_socket
#   define scheduler_add_client_socket  scheduler_fake_add_client_socket
#   define scheduler_del_socket         scheduler_fake_del_socket
#   define scheduler_loop               scheduler_fake_loop
#endif

#endif /* H_SCHEDULER_H */

