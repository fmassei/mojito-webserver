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

#ifdef HAVE_SYS_SOCKET_H
#   include <sys/socket.h>
#else
#   error no sys/socket.h found in your system!
#endif
#ifdef HAVE_NETINET_IN_H
#   include <netinet/in.h>
#else
#   error no netinet/in.h found in your system!
#endif
#ifdef HAVE_ARPA_INET_H
#   include <arpa/inet.h>
#else
#   error no arpa/inet.h found in your system!
#endif

#define INVALID_SOCKET  -1
typedef int socket_t;

/* server start/stop/accept functions */
int mjt_server_start(int port, int qsize);
socket_t mjt_server_accept(char **in_ip);
int mjt_server_stop();

#endif /* H_MJT_SOCKET_H */
