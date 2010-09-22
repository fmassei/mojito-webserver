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
#include "socket_unit.h"

t_socket_unit_s *socket_unit_create(int qsize)
{
    t_socket_unit_s *ret;
    int i;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        goto bad_exit;
    }
    ret->queue_size = qsize;
    if ((ret->connect_list = xcalloc(qsize, sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        goto bad_exit;
    }
    if ((ret->reqs = xcalloc(qsize, sizeof(*(ret->reqs))))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        goto bad_exit;
    }
    if ((ret->resps = xcalloc(qsize, sizeof(*(ret->resps))))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        goto bad_exit;
    }
    if ((ret->socket_states = xcalloc(qsize, sizeof(*(ret->socket_states))))
                                                                    ==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        goto bad_exit;
    }
    if ((ret->mtx = mmp_thr_mtx_create())==MMP_THRMTX_INVALID) {
        mmp_setError(MMP_ERR_SYNC);
        goto bad_exit;
    }
    if ((ret->sleep_evt = mmp_thr_evt_create())==MMP_THREVT_INVALID) {
        mmp_setError(MMP_ERR_SYNC);
        goto bad_exit;
    }
    for (i=0; i<qsize; ++i) {
        ret->reqs[i] = NULL;
        ret->resps[i] = NULL;
        ret->connect_list[i] = SOCKET_INVALID;
        ret->socket_states[i] = SOCKET_STATE_NOTPRESENT;
    }
    ret->nsockets = 0;
    ret->to.tv_sec = 20;        /* move to config */
    ret->to.tv_usec = 0;
    ret->newdata_cback = NULL;
    ret->state = SOCKET_UNIT_STATE_RUNNING;
#ifndef _WIN32
    ret->highest_socket = -1;
#endif
    return ret;
bad_exit:
    if (ret!=NULL)
        socket_unit_destroy(&ret);
    return NULL;
}

void socket_unit_destroy(t_socket_unit_s **su)
{
    int i;
    if (su==NULL || *su==NULL) return;
    if ((*su)->connect_list!=NULL) xfree((*su)->connect_list);
    if ((*su)->socket_states!=NULL) xfree((*su)->socket_states);
    if ((*su)->reqs!=NULL) {
        for (i=0; i<(*su)->queue_size; ++i)
            if ((*su)->reqs[i]!=NULL)
                request_destroy(&((*su)->reqs[i]));
        xfree((*su)->reqs);
    }
    if ((*su)->resps!=NULL) {
        for (i=0; i<(*su)->queue_size; ++i)
            if ((*su)->resps[i]!=NULL)
                response_destroy(&((*su)->resps[i]));
        xfree((*su)->resps);
    }
    if ((*su)->mtx!=MMP_THRMTX_INVALID) mmp_thr_mtx_close(&(*su)->mtx);
    xfree(*su);
    *su = NULL;
}

#ifdef _WIN32
#   pragma warning(push)
#   pragma warning(disable:4127) /* disable win32 warning for FD_SET */
#endif
static void build_select_list(t_socket_unit_s *su)
{
    int i;
    FD_ZERO(&su->sockets);
    for (i=0; i<su->queue_size; ++i) {
        if (su->connect_list[i]!=SOCKET_INVALID) {
            FD_SET(((unsigned int)(su->connect_list[i])), (&su->sockets));
        } else break;
    }
}
#ifdef _WIN32
#   pragma warning(pop)
#endif

int socket_unit_add_connection(t_socket_unit_s *su, t_socket socket)
{
    int i, ret;
    if (su==NULL || socket==SOCKET_INVALID) {
        mmp_setError(MMP_ERR_PARAMS);
        return -1;
    }
    if (mmp_thr_mtx_lock(su->mtx)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_SYNC);
        return -1;
    }
    ret = -1;
    for (i=0; i<su->queue_size; ++i) {
        if (su->connect_list[i]==SOCKET_INVALID) {
            su->connect_list[i] = socket;
            ret = i;
            break;
        }
    }
    su->socket_states[ret] = SOCKET_STATE_READREQUEST;
    su->reqs[ret] = request_create();
    su->resps[ret] = response_create();
    su->resps[ret]->sock = socket;
    ++su->nsockets;
#ifndef _WIN32
    if (socket>su->highest_socket)
        su->highest_socket = socket;
#endif
    if (su->nsockets==1 && su->state==SOCKET_UNIT_STATE_SLEEPING) {
        if (mmp_thr_evt_signal(su->sleep_evt)!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_SYNC);
        } else {
            su->state = SOCKET_UNIT_STATE_RUNNING;
        }
    }
    if (mmp_thr_mtx_release(su->mtx)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_SYNC);
        return -1;
    }
    return ret;
}

ret_t socket_unit_del_connection(t_socket_unit_s *su, int slot)
{
#ifndef _WIN32
    int i;
    t_socket socket;
#endif
    if (su==NULL || slot<0) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if (mmp_thr_mtx_lock(su->mtx)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_SYNC);
        return MMP_ERR_SYNC;
    }
#ifndef _WIN32
    socket = su->connect_list[slot];
#endif
    su->connect_list[slot] = SOCKET_INVALID;
    request_destroy(&su->reqs[slot]);
    response_destroy(&su->resps[slot]);
    --su->nsockets;
#ifndef _WIN32
    if (socket==su->highest_socket) {
        su->highest_socket = -1;
        for (i=0; i<su->nsockets; ++i) {
            if (su->connect_list[i]>su->highest_socket)
                su->highest_socket = su->connect_list[i];
        }
    }
#endif
    if (mmp_thr_mtx_release(su->mtx)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_SYNC);
        return MMP_ERR_SYNC;
    }
    return MMP_ERR_OK;
}

static void read_sockets(t_socket_unit_s *su)
{
    int i;
    for (i=0; i<su->nsockets; ++i) {
        if (FD_ISSET(su->connect_list[i], &su->sockets)) {
            if (su->newdata_cback!=NULL) {
                su->newdata_cback(i, su);
            }
        }
    }
}

ret_t socket_unit_select_loop(t_socket_unit_s *su)
{
    int read_socks, hs;
    if (su==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if (mmp_thr_mtx_lock(su->mtx)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_SYNC);
        return MMP_ERR_SYNC;
    }
    build_select_list(su);
    if (su->nsockets<=0) {
        if (mmp_thr_mtx_release(su->mtx)!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_SYNC);
            return MMP_ERR_SYNC;
        }
        su->state = SOCKET_UNIT_STATE_SLEEPING;
        if (mmp_thr_evt_wait(su->sleep_evt)!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_SYNC);
            return MMP_ERR_SYNC;
        }
        return MMP_ERR_OK;
    }
#ifndef _WIN32
    hs = su->highest_socket+1;
#else
    hs = 0;
#endif
reselect:
    read_socks = mmp_socket_server_select(hs, &su->sockets, NULL,NULL, &su->to);
    if (read_socks==-1 && errno==EINTR)
        goto reselect; /* interrupt */
    if (mmp_thr_mtx_release(su->mtx)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_SYNC);
        return MMP_ERR_SYNC;
    }
    if (read_socks<0) {
        mmp_setError(MMP_ERR_SOCKET);
        return MMP_ERR_SOCKET;
    }
    read_sockets(su);
    return MMP_ERR_OK;
}

