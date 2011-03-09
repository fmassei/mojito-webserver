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
#ifndef H_HEADER_CODES_H
#define H_HEADER_CODES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_date.h>
#include "types.h"
#include "config_manager.h"
#include "response_type.h"
#include "request.h"

void header_push_code(t_response_s *resp, t_hresp_e code,
                                        t_request_protocol_e proto_version);

void header_send(t_response_s *resp);

void header_kill_w_code(t_response_s *resp, t_hresp_e code,
                                        t_request_protocol_e proto_version);

#endif /* H_HEADER_CODES_H */