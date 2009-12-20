/*
    Copyright 2009 Francesco Massei

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

#include "mjt_socket.h"

static socket_t srv_sock = INVALID_SOCKET;
static int queuesize;

/* accept an incoming request */
socket_t mjt_server_accept(char **in_ip)
{
    struct sockaddr_in addr;
    socket_t ret;
    socklen_t addrlen = sizeof(addr);
    if ((ret = accept(srv_sock, (struct sockaddr *)&addr, &addrlen))==-1)
        return INVALID_SOCKET;
    if ((*in_ip = strdup(inet_ntoa(addr.sin_addr)))==NULL) {
        close(ret);
        return INVALID_SOCKET;
    }
    return ret;
}

/* start the server */
int mjt_server_start(int port, int qsize)
{
    struct sockaddr_in sa;
    int raddr = 1;

    queuesize = qsize;
    if ((srv_sock = socket(AF_INET, SOCK_STREAM, 0))==INVALID_SOCKET)
        return -1;
    setsockopt(srv_sock, SOL_SOCKET, SO_REUSEADDR, &raddr, sizeof(raddr));
    memset((char*)&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(port);
    if (bind(srv_sock, (struct sockaddr*)&sa, sizeof(sa))<0) {
        close(srv_sock);
        return -1;
    }
    if (listen(srv_sock, queuesize)<0) {
        close(srv_sock);
        return -1;
    }
    return 0;
}

/* stop the server */
int mjt_server_stop()
{
    if (srv_sock!=INVALID_SOCKET)
        close(srv_sock);
    srv_sock = INVALID_SOCKET;
    return 0;
}

