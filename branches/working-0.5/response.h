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
#ifndef H_RESPONSE_H
#define H_RESPONSE_H

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "response_type.h"
#include "resp_headers.h"
#include "modules.h"
#include "mime.h"
#include "filter_manager.h"
#include "socket_unit.h"

void response_init(t_response_s *res);
void response_drop(t_response_s *res);
void response_send(t_socket_unit_s *su);

#endif /* H_RESPONSE_H */
