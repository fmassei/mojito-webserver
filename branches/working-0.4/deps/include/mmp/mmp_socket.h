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
#ifndef H_MMP_SOCKET_H
#define H_MMP_SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef _WIN32
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <sys/time.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <fcntl.h>
#   include <unistd.h>
    typedef int t_socket;
#   define  SOCKET_INVALID  (-1)
#   define SOCKET_ERROR -1
#else
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   include <winsock2.h>
#   include <windows.h>
#   include <ws2tcpip.h>
    typedef SOCKET t_socket;
#   define SOCKET_INVALID   INVALID_SOCKET
    /* socket error is defined */
#endif

#include "mmp_trace.h"
#include "mmp_memory.h"
#include "mmp_string.h"

ret_t mmp_socket_initSystem(void);
ret_t mmp_socket_finiSystem(void);
ret_t mmp_socket_server_start(int port, int qsize, t_socket *sock);
ret_t mmp_socket_close(t_socket *sock, int shut);
ret_t mmp_socket_server_accept(t_socket *listen_sock, t_socket *out, char **ip);
int mmp_socket_server_select(int nfds, fd_set *rd, fd_set *wd, fd_set *ex,
                                                            struct timeval *to);
int mmp_socket_read(t_socket *sock, void *buf, size_t len);
int mmp_socket_write(t_socket *sock, void *buf, size_t len);

int mmp_socket_is_block_last_error(void);

#endif /* H_MMP_SOCKET_H */
