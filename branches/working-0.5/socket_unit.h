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
#ifndef H_SOCKET_UNIT_H
#define H_SOCKET_UNIT_H

#include <mmp/mmp_socket.h>
#include "types.h"
#include "response.h"
#include "request.h"

typedef enum socket_state_e {
    SOCKET_STATE_NOTPRESENT     = 0,
    SOCKET_STATE_READREQUEST    = 1,
    SOCKET_STATE_WRITERESPONSE  = 2,
} t_socket_state_e;

struct socket_unit_s {
    t_socket socket;
    t_socket_state_e state;
    t_request_s req;
    t_response_s res;
};

void socket_unit_init(t_socket_unit_s *su);
void socket_unit_drop(t_socket_unit_s *su);

#endif /* H_SOCKET_UNIT_H */
