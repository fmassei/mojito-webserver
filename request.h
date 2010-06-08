#ifndef H_REQUEST_H
#define H_REQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <mmp/mmp_memory.h>
#include <mmp/mmp_trace.h>

/* variables needed for request parsing */
typedef struct request_parse_s {
    int reline;
    int rb, mr, len, pos;
    char c, *buf, *cur_head;
} t_request_parse_s;

/* the main request object */
typedef struct request_s {
    t_request_parse_s   parse;
} t_request_s;

t_request_s *request_create(void);
void request_destroy(t_request_s **request);

#endif /* H_REQUEST_H */
