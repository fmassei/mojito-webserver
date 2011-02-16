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
#include "utils.h"

void socket_unit_init(t_socket_unit_s *su)
{
    su->socket = INVALID_SOCKET;
    su->state = SOCKET_STATE_NOTPRESENT;
    request_init(&su->req);
    response_init(&su->res);
    bzero(&su->aio, sizeof(su->aio));
}

void socket_unit_drop(t_socket_unit_s *su)
{

}

