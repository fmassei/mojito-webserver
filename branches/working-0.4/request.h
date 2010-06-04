#ifndef H_REQUEST_H
#define H_REQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <mmp/mmp_memory.h>
#include <mmp/mmp_trace.h>

typedef struct request_s {
} t_request_s;

t_request_s *request_create(void);
void request_destroy(t_request_s **request);

#endif /* H_REQUEST_H */
