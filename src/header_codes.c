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
#include "header_codes.h"
#include "../common_src/resp_headers.h"

/* protocol strings */
static const char HTTP10[] = "HTTP/1.0 ";
static const char HTTP11[] = "HTTP/1.1 ";

/* some status responses */
static const char *RESP[] = {
        "200 OK\r\n",
        "404 Not Found\r\n",
        "406 Not Acceptable\r\n",
        "500 Internal Server Error\r\n",
        "501 Not Implemented\r\n"
    };

void header_push_code(t_response_s *resp, t_hresp_e code,
                                        t_request_protocol_e proto_version)
{
    char datebuf[30];
    const t_config_s *config;
    resp->resbuf[0] = '\0';
    /* RFC2145 - conservative approach */
    if (proto_version==REQUEST_PROTOCOL_HTTP11)
        strcat(resp->resbuf, HTTP11);
    else
        strcat(resp->resbuf, HTTP10);
    strcat(resp->resbuf, RESP[code]);
    mmp_time_1123_format(time(NULL), datebuf, sizeof(datebuf));
    header_send_hs(resp, "Date", datebuf);
    if ((config = config_get())!=NULL && config->server->server_meta!=NULL) {
        header_send_hs(resp, "Server", config->server->server_meta);
    }
    resp->final_code = code;
}

void header_send(t_response_s *resp)
{
    strcat(resp->resbuf, "\r\n");
    mmp_socket_write(&resp->sock, resp->resbuf, strlen(resp->resbuf));
}

void header_kill_w_code(t_response_s *resp, t_hresp_e code,
                                            t_request_protocol_e proto_version)
{
    header_push_code(resp, code, proto_version);
    header_push_contentlength(resp, 0);
    header_send(resp);
    resp->final_code = code;
    resp->final_data_sent = 0;
}

