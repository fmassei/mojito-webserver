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
#include <mmp/mmp_files.h>

void request_init(t_request_s *req, int min_keep)
{
    req->parse.buf = req->parse.cur_head = NULL;
    req->parse.status = REQUEST_PARSE_STATUS_HEAD;
    req->method = REQUEST_METHOD_UNKNOWN;
    req->protocol = REQUEST_PROTOCOL_UNKNOWN;
    req->URI = NULL;
    req->keeping_alive = 0;
    req->content_length = 0;
    req->content_type = NULL;
    req->accept_encoding = NULL;
    req->referer = req->user_agent = req->first_line = NULL;
    req->post_fd = -1;
}

void request_drop(t_request_s *req, int min_keep)
{
    if (req==NULL) return;
    if (req->parse.buf!=NULL)
        MMP_XFREE_AND_NULL(req->parse.buf);
    if (req->accept_encoding!=NULL)
        qhead_list_destroy(&req->accept_encoding);
    if (req->URI!=NULL)
        MMP_XFREE_AND_NULL(req->URI);
    if (req->referer!=NULL)
        MMP_XFREE_AND_NULL(req->referer);
    if (req->user_agent!=NULL)
        MMP_XFREE_AND_NULL(req->user_agent);
    if (req->first_line!=NULL)
        MMP_XFREE_AND_NULL(req->first_line);
}

