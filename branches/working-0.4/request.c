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

static void request_parse_init(t_request_parse_s *rp)
{
    rp->buf = rp->cur_head = NULL;
    rp->status = REQUEST_PARSE_STATUS_HEAD;
}

t_request_s *request_create(void)
{
    t_request_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    request_parse_init(&ret->parse);
    ret->method = REQUEST_METHOD_UNKNOWN;
    ret->protocol = REQUEST_PROTOCOL_UNKNOWN;
    ret->URI = NULL;
    ret->keeping_alive = 0;
    ret->content_length = 0;
    ret->content_type = NULL;
    ret->accept_encoding = NULL;
    return ret;
}

void request_destroy(t_request_s **request)
{
    if (request==NULL || *request==NULL) return;
    if ((*request)->accept_encoding!=NULL)
        qhead_list_destroy(&((*request)->accept_encoding));
    xfree(*request);
    *request = NULL;
}

