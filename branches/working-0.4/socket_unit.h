#ifndef H_SOCKET_UNIT_H
#define H_SOCKET_UNIT_H

#include <stdio.h>
#include <stdlib.h>
#include <mmp/mmp_memory.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_socket.h>
#include <mmp/mmp_sync.h>
#include "request.h"

typedef enum socket_state_e {
    SOCKET_STATE_NOTPRESENT     = 0,
    SOCKET_STATE_READREQUEST    = 1,
    SOCKET_STATE_WRITERESPONSE  = 2
} t_socket_state_e;

typedef enum socket_unit_state_e {
    SOCKET_UNIT_STATE_RUNNING   = 0,
    SOCKET_UNIT_STATE_CLOSING   = 1
} t_socket_unit_state_e;

/* forward typedef declaration */
typedef struct socket_unit_s t_socket_unit_s;

/* server general function pointer type for callbacks */
typedef ret_t(*t_sckunit_fptr)(int, t_socket_unit_s *);

/* socket unit structure */
struct socket_unit_s {
    int *connect_list;                  /* list of connections */
    int nsockets;                       /* number of sockets present */
    int highest_socket;                 /* highest socket number. This is not
                                         * used in win32 select(), but we need
                                         * it in the posix one. */
    int queue_size;                     /* size of the connection queue */
    fd_set sockets;                     /* all the sockets */
    struct timeval to;                  /* time out for fd selecting */
    mmp_thr_mtx_t mtx;                  /* unit mutex */
    t_request_s **reqs;                 /* list of requests */
    t_socket_state_e *socket_states;    /* logical state of the sockets */
    t_socket_unit_state_e state;        /* state of the socket unit */
    t_sckunit_fptr  newdata_cback;  /* data ready callback */
};

t_socket_unit_s *socket_unit_create(int qsize);
void socket_unit_destroy(t_socket_unit_s **su);
int socket_unit_add_connection(t_socket_unit_s *su, socket_t socket);
ret_t socket_unit_select_loop(t_socket_unit_s *su);

#endif /* H_SOCKET_UNIT_H */
