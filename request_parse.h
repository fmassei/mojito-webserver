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
#ifndef H_REQUEST_PARSE_H
#define H_REQUEST_PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <mmp/mmp_socket.h>
#include "request.h"

typedef enum request_parse_e {
    REQUEST_PARSE_CLOSECONN = 0,
    REQUEST_PARSE_FINISH    = 1,
    REQUEST_PARSE_CONTINUE  = 2,
    REQUEST_PARSE_ERROR     = 3
} t_request_parse_e;

t_request_parse_e request_parse_read(t_socket *sock, t_request_s *req);

#endif /* H_REQUEST_PARSE_H */
