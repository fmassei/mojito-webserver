#ifndef H_REQUEST_PARSE_H
#define H_REQUEST_PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <mmp/mmp_socket.h>
#include "request.h"

int request_parse_read(socket_t *sock, t_request_s *req);

#endif /* H_REQUEST_PARSE_H */
