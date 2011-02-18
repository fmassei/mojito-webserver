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
#include "request.h"

void request_create(t_request_s *req)
{
    request_parse_init(&ret->parse);
    req->parse.buf = req->parse.cur_head = NULL;
    req->parse.status = REQUEST_PARSE_STATUS_HEAD;
    req->method = REQUEST_METHOD_UNKNOWN;
    req->protocol = REQUEST_PROTOCOL_UNKNOWN;
    req->URI = NULL;
    req->keeping_alive = 0;
    req->content_length = 0;
    req->content_type = NULL;
    req->accept_encoding = NULL;
}

void request_drop(t_request_s *req)
{
    if (request==NULL) return;
    if (req->accept_encoding!=NULL)
        qhead_list_destroy(&req->accept_encoding);
}

