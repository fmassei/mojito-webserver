#include "request_parse.h"

#define SOCKBUFSIZE 8196

int request_parse_read(socket_t *sock, t_request_s *req)
{
    int reline;
    int rb, mr, len, pos;
    char c, *buf, *cur_head;
    len = pos = 0;
    while(1) {
        mr = SOCKBUFSIZE + pos;
        if (len<mr) {
            reline = (int)cur_head - (int)buf;
            buf = xrealloc(buf, mr);
            cur_head = (char*)((int)buf + reline);
            len = mr;
        }
        rb = socket_read(sock, buf+pos, SOCKBUFSIZE);
        if (rb<=0)
            break;
        return 0;
    }
    return 1;
}

