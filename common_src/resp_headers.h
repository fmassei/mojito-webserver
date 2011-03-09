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
#include "../src/types.h"
#include "../src/response_type.h"
#include "../src/request.h"

void header_send_hs(t_response_s *resp, const char *h, const char *s);
void header_send_hl(t_response_s *resp, const char *h, long l);

void header_push_contentlength(t_response_s *resp, long len);
void header_push_contenttype(t_response_s *resp, const char *name);
void header_push_contentencoding(t_response_s *resp, const char *name);

const char *request_method_string(t_request_method_e method);

#endif /* H_RESP_HEADERS_H */
