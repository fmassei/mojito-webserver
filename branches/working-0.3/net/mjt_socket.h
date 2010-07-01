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

#ifndef H_MJT_SOCKET_H
#define H_MJT_SOCKET_H

#include <mjt_types.h>

#if HAVE_SYS_SOCKET_H
#   include <sys/socket.h>
#endif
#if HAVE_NETINET_IN_H
#   include <netinet/in.h>
#endif
#if HAVE_ARPA_INET_H
#   include <arpa/inet.h>
#endif
#if HAVE_SYS_SELECT_H
#   include <sys/select.h>
#endif

#if !HAVE_SELECT
#   error no select() found!
#endif

#define INVALID_SOCKET  -1
typedef int_t socket_t;

/* get the system preferred socket buffer size */
int_t mjt_socket_getbuffersize();

/* wait on a socket for a specific timeout */
int_t mjt_socket_waitonalive(socket_t sock, int_t seconds);

#endif /* H_MJT_SOCKET_H */