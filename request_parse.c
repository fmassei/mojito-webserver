#include "request_parse.h"

#define SOCKBUFSIZE 8196    /* TODO: calculate this one! */

static int parse_first_line(t_request_s *req, char *line) {return 0;}
static int parse_header_line(t_request_s *req, char *line) {return 0;}
/* parse an option */
static int parse_option(t_request_s *req, char *line)
{
    if (req->method==0)
        return parse_first_line(req, line);
    else
        return parse_header_line(req, line);
}

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
            return REQUEST_PARSE_CLOSECONN;
        }
        /* BOA-like loop. An insane mess, but pretty clear after two or three
         * hours spent on reading this twenty lines. Good luck. */
        while(req->parse.rb-->0) {
            req->parse.c = *(req->parse.buf+req->parse.pos);
            if (req->parse.c=='\0')
                continue;
            switch(req->parse.status) {
            case REQUEST_PARSE_STATUS_HEAD:
                if (req->parse.c=='\r')
                    req->parse.status = REQUEST_PARSE_STATUS_CR1;
                else if (req->parse.c=='\n')
                    req->parse.status = REQUEST_PARSE_STATUS_LF1;
                break;
            case REQUEST_PARSE_STATUS_CR1:
                if (req->parse.c=='\n')
                    req->parse.status = REQUEST_PARSE_STATUS_LF1;
                else if (req->parse.c=='\r')
                    req->parse.status = REQUEST_PARSE_STATUS_HEAD;
                break;
            case REQUEST_PARSE_STATUS_LF1:
                if (req->parse.c=='\r')
                    req->parse.status = REQUEST_PARSE_STATUS_CR2;
                else if (req->parse.c=='\n')
                    req->parse.status = REQUEST_PARSE_STATUS_BODY;
                else
                    req->parse.status = REQUEST_PARSE_STATUS_HEAD;
                break;
            case REQUEST_PARSE_STATUS_CR2:
                if (req->parse.c=='\n')
                    req->parse.status = REQUEST_PARSE_STATUS_BODY;
                else if (req->parse.c=='\r')
                    req->parse.status = REQUEST_PARSE_STATUS_HEAD;
                break;
            }
            ++req->parse.pos;
            if (req->parse.status==REQUEST_PARSE_STATUS_LF1) {
                /* header found */
                req->parse.buf[req->parse.pos-2] = '\0';
                parse_option(req, req->parse.cur_head);
                req->parse.cur_head = req->parse.buf+req->parse.pos;
            } else if (req->parse.status==REQUEST_PARSE_STATUS_BODY) {
            }
        }
        /* return REQUEST_PARSE_FINISH; somewhere */ 
    }
    return REQUEST_PARSE_ERROR; /* this should never happen */
}

