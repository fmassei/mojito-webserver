#ifndef H_REQUEST_PARSE_H
#define H_REQUEST_PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <mmp/mmp_socket.h>
#include "request.h"

typedef enum request_parse_e {
    REQUEST_PARSE_CLOSECONN = 0,
    REQUEST_PARSE_FINISH    = 1,
    REQUEST_PARSE_CONTINUE  = 2,
    REQUEST_PARSE_ERROR     = 3
} t_request_parse_e;

t_request_parse_e request_parse_read(socket_t *sock, t_request_s *req);

#endif /* H_REQUEST_PARSE_H */
