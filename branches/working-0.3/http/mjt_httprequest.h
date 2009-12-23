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

#ifndef H_MJT_HTTPREQUEST_H
#define H_MJT_HTTPREQUEST_H

#include <mjt_types.h>
#include <lib/mjt_alloc.h>
#include <lib/mjt_logger.h>
#include <lib/mjt_rw.h>
#include <net/mjt_socket.h>
#include "mjt_headerwquality.h"

#if !HAVE_MKSTEMP
#   error no mkstemp found!
#endif

/* method defines */
#define HTTP_METHOD_GET     1
#define HTTP_METHOD_HEAD    2
#define HTTP_METHOD_POST    3

/* http protocol version */
#define HTTP_VER_UNKNOWN    0
#define HTTP_VER_10         1
#define HTTP_VER_11         2

/* header parsing result */
#define REQUEST_PARSE_OK            0
#define REQUEST_PARSE_ERROR         -1
#define REQUEST_PARSE_INCOMPLETE    1   /* probably the client closed the 
                                        * connection in the middle of the
                                        * parsing process */

/* http request structure */
struct mjt_request_s {
    char_t  *in_ip;
    char_t  *URI;
    int_t   http_method;
    int_t   proto_version;
    char_t  *content_type;
    long_t  content_length;
    struct mjt_qhead_s *accept_encoding;
    char_t  *page, *query_string;   /* the splitted URL+query_string ptrs */
    int_t   post_fd;    /* post file descriptor */
    bool_t   can_keepalive;  /* can the connection associated with this request
                                be kept alive? */
    /* final file data, if any */
    char_t  *abs_filename;
    struct stat *sb;
};

BEGIN_C_DECLS

extern const char_t *mjt_get_http_method_name(int_t http_method);

extern struct mjt_request_s *mjt_request_create();
extern int_t mjt_request_read(int_t sock, struct mjt_request_s *req);
extern void mjt_request_destroy(struct mjt_request_s **req);

END_C_DECLS

#endif /* H_MJT_HTTPREQUEST_H */
