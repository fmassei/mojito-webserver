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
#ifndef H_REQUEST_H
#define H_REQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <mmp/mmp_memory.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_files.h>
#include "types.h"
#include "header_w_quality.h"

typedef enum request_parse_status_e {
        REQUEST_PARSE_STATUS_HEAD,
        REQUEST_PARSE_STATUS_CR1,
        REQUEST_PARSE_STATUS_LF1,
        REQUEST_PARSE_STATUS_CR2,
        REQUEST_PARSE_STATUS_BODY
} t_request_parse_status_e;

typedef enum request_method_e {
        REQUEST_METHOD_UNKNOWN,
        REQUEST_METHOD_GET,
        REQUEST_METHOD_HEAD,
        REQUEST_METHOD_POST
} t_request_method_e;

typedef enum request_protocol_e {
        REQUEST_PROTOCOL_UNKNOWN,
        REQUEST_PROTOCOL_HTTP10,
        REQUEST_PROTOCOL_HTTP11
} t_request_protocol_e;

/* variables needed for request parsing */
typedef struct request_parse_s {
    int reline;
    int rb, mr, len, pos;
    char c, *buf, *cur_head;
    t_request_parse_status_e status;
} t_request_parse_s;

/* the main request object */
struct request_s {
    t_request_parse_s parse;
    t_request_method_e method;
    t_request_protocol_e protocol;
    char *URI;
    int keeping_alive;
    size_t content_length;
    char *content_type;
    t_qhead_list_s *accept_encoding;
    /* filled by response */
    char *page, *query_string, *abs_filename;
};

void request_init(t_request_s *req);
void request_drop(t_request_s *req);

#endif /* H_REQUEST_H */
