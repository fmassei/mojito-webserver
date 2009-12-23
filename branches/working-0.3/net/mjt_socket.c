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

/* get the system preferred socket buffer size */
int_t mjt_socket_getbuffersize()
{
    /* FIXME: that's broken right now */
    return 8196;
}

/* wait on a socket for a specific timeout */
int_t mjt_socket_waitonalive(socket_t sock, int_t seconds)
{
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    return select(sock+1, &rfds, NULL, NULL, &tv);
}

