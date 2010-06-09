#include "request_parse.h"

#define SOCKBUFSIZE 8196    /* TODO: calculate this one! */

t_request_parse_e request_parse_read(socket_t *sock, t_request_s *req)
{
    req->parse.len = req->parse.pos = 0;
#ifdef _WIN32
#   pragma warning(push)
#   pragma warning(disable:4127)
#endif
    while(1) {
#ifdef _WIN32
#   pragma warning(pop)
#endif
        req->parse.mr = SOCKBUFSIZE + req->parse.pos;
        if (req->parse.len<req->parse.mr) {
            req->parse.reline =(int)(req->parse.cur_head)-(int)(req->parse.buf);
            req->parse.buf = xrealloc(req->parse.buf, req->parse.mr);
            req->parse.cur_head =(char*)((int)req->parse.buf+req->parse.reline);
            req->parse.len = req->parse.mr;
        }
        req->parse.rb =
                socket_read(sock, req->parse.buf+req->parse.pos, SOCKBUFSIZE);
        if (req->parse.rb==SOCKET_ERROR) {
            if (socket_is_block_last_error())
                return REQUEST_PARSE_CONTINUE;
            return REQUEST_PARSE_ERROR;
        }
        if (req->parse.rb==0) {
            return REQUEST_PARSE_FINISH;
        }
        return REQUEST_PARSE_FINISH;
    }
    return REQUEST_PARSE_ERROR; /* this should never happen */
}

