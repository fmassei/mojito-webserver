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
#include "resp_headers.h"

void header_send_hs(t_response_s *resp, const char *h, const char *s)
{
    sprintf(resp->tmpbuf, "%s: %s\r\n", h, s);
    strcat(resp->resbuf, resp->tmpbuf);
}

void header_send_hl(t_response_s *resp, const char *h, long l)
{
    sprintf(resp->tmpbuf, "%s: %lu\r\n", h, l);
    strcat(resp->resbuf, resp->tmpbuf);
}

void header_push_contentlength(t_response_s *resp, long len)
{
    header_send_hl(resp, "Content-Length", len);
    resp->content_length_sent = 1;
}

void header_push_contenttype(t_response_s *resp, const char *name)
{
    header_send_hs(resp, "Content-Type", name);
}

void header_push_contentencoding(t_response_s *resp, const char *name)
{
    if (!strcmp(name, "identity"))
        return;
    header_send_hs(resp, "Content-Encoding", name);
}

const char *request_method_string(t_request_method_e method)
{
    switch(method) {
    case REQUEST_METHOD_GET:
        return "GET";
    case REQUEST_METHOD_HEAD:
        return "HEAD";
    case REQUEST_METHOD_POST:
        return "POST";
    case REQUEST_METHOD_UNKNOWN:
    default:
        return "UNKNOWN";
    }
}

