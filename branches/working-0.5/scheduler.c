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
    ev.events = EPOLLIN | EPOLLET;
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
    if ((nfds = epoll_wait(sched_id, s_events, s_pool_size, -1))==-1) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    for (n=0; n<nfds; ++n)
        cback_fp(s_events[n].data.fd);
    return MMP_ERR_OK;
}

