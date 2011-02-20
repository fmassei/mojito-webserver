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
#include "scheduler.h"
#include <sys/epoll.h>

static struct epoll_event *s_events;
static size_t s_pool_size;

t_sched_id scheduler_create(size_t pool_size)
{
    t_sched_id epollfd;
    s_pool_size = pool_size;
    if ((s_events = xcalloc(pool_size, sizeof(*s_events)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return -1;
    }
    if ((epollfd = epoll_create(pool_size))==-1) {
        mmp_setError(MMP_ERR_GENERIC);
        return -1;
    }
    return epollfd;
}

void scheduler_destroy(t_sched_id sched_id)
{
}

ret_t scheduler_add_listen_socket(t_sched_id sched_id, t_socket sock)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    if (epoll_ctl(sched_id, EPOLL_CTL_ADD, sock, &ev)==-1) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    return MMP_ERR_OK;
}

ret_t scheduler_add_client_socket(t_sched_id sched_id, t_socket sock)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
    ev.data.fd = sock;
    if (epoll_ctl(sched_id, EPOLL_CTL_ADD, sock, &ev)==-1) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    return MMP_ERR_OK;
}

ret_t scheduler_del_socket(t_sched_id sched_id, t_socket sock)
{
    struct epoll_event ev;
    if (epoll_ctl(sched_id, EPOLL_CTL_DEL, sock, &ev)!=-1) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    return MMP_ERR_OK;
}

ret_t scheduler_loop(t_sched_id sched_id, void(*cback_fp)(t_socket))
{
    int nfds, n;
repoll:
    if ((nfds = epoll_wait(sched_id, s_events, s_pool_size, -1))==-1) {
#ifndef NDEBUG
        if (errno==EINTR) goto repoll; /* FIXME: remove this */
#endif
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    for (n=0; n<nfds; ++n)
        cback_fp(s_events[n].data.fd);
    return MMP_ERR_OK;
}

