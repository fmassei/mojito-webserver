#include "socket_unit.h"

t_socket_unit_s *socket_unit_create(int qsize)
{
    t_socket_unit_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    if ((ret->connect_list = xcalloc(qsize, sizeof(*ret)))==NULL) {
        xfree(ret);
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    ret->nsockets = 0;
    ret->queue_size = qsize;
    ret->to.tv_sec = 1;     /* move to config */
    ret->to.tv_usec = 0;
    ret->newdata_cback = NULL;
    return ret;
}

void socket_unit_destroy(t_socket_unit_s **su)
{
    if (su==NULL || *su==NULL) return;
    if ((*su)->connect_list!=NULL) xfree((*su)->connect_list);
    xfree(*su);
    *su = NULL;
}

static void build_select_list(t_socket_unit_s *su)
{
    int i;
    FD_ZERO(&su->sockets);
    for (i=0; i<su->queue_size; ++i) {
        if (su->connect_list[i]!=0) {
            FD_SET(su->connect_list[i], &su->sockets);
        }
    }
}

int socket_unit_add_connection(t_socket_unit_s *su, socket_t socket)
{
    int i;
    if (su==NULL || socket==SOCKET_INVALID) {
        mmp_setError(MMP_ERR_PARAMS);
        return -1;
    }
    ++su->nsockets;
    for (i=0; i<su->queue_size; ++i) {
        if (su->connect_list[i]==0) {
            su->connect_list[i] = socket;
            return i;
        }
    }
    return -1;  /* full */
}

static void read_sockets(t_socket_unit_s *su)
{
    int i;
    for (i=0; i<su->queue_size; ++i) {
        if (FD_ISSET(su->connect_list[i], &su->sockets)) {
            if (su->newdata_cback!=NULL) {
                su->newdata_cback(i, su->connect_list[i]);
            }
        }
    }
}

ret_t socket_unit_select_loop(t_socket_unit_s *su)
{
    int read_socks;
    if (su==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    build_select_list(su);
    read_socks = socket_server_select(su->nsockets, &su->sockets, (fd_set*)0,
                                                        (fd_set*)0, &su->to);
    if (read_socks<0) {
        mmp_setError(MMP_ERR_SOCKET);
        return MMP_ERR_SOCKET;
    }
    read_sockets(su);
    return MMP_ERR_OK;
}

