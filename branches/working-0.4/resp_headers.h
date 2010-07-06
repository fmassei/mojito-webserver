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
#ifndef H_RESP_HEADERS_H
#define H_RESP_HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_date.h>
#include "response.h"

typedef enum hresp_e {
    HRESP_200   =   0,
    HRESP_404   =   1,
    HRESP_406   =   2,
    HRESP_500   =   3,
    HRESP_501   =   4
} t_hresp_e;

void header_push_code(t_response_s *resp, t_hresp_e code, int proto_version);

void header_push_contentlength(t_response_s *resp, long len);
void header_push_contenttype(t_response_s *resp, char *name);
void header_push_contentencoding(t_response_s *resp, char *name);

void header_part_send(t_response_s *resp);
void header_send(t_response_s *resp);

void header_kill_w_code(t_response_s *resp, t_hresp_e code);

#endif /* H_RESP_HEADERS_H */
