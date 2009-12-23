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

#ifndef H_MJT_SERVER_H
#define H_MJT_SERVER_H

#include <mjt_types.h>
#include "mjt_socket.h"

#if !HAVE_BIND || !HAVE_ACCEPT || !HAVE_SOCKET || !HAVE_LISTEN ||  \
        !HAVE_SETSOCKOPT
#   error some needed functions not found!
#endif

/* server start/stop/accept functions */
int_t mjt_server_start(int_t port, int_t qsize);
socket_t mjt_server_accept(char_t **in_ip);
int_t mjt_server_stop();

#endif /* H_MJT_SERVER_H */
