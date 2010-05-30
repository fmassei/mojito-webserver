#ifndef H_SOCKET_UNIT_H
#define H_SOCKET_UNIT_H

#include <stdio.h>
#include <stdlib.h>
#include <mmp/mmp_memory.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_socket.h>

/* server general function pointer type for callbacks */
typedef ret_t(*t_sckunit_fptr)(int, socket_t);

/* socket unit structure */
typedef struct socket_unit_s {
    int *connect_list;  /* list of connections */
    int nsockets;       /* number of sockets present */
    int queue_size;     /* size of the connection queue */
    fd_set sockets;     /* all the sockets */
    struct timeval to;  /* time out for fd selecting */
    t_sckunit_fptr  newdata_cback;  /* data ready callback */
} t_socket_unit_s;

t_socket_unit_s *socket_unit_create(int qsize);
void socket_unit_destroy(t_socket_unit_s **su);
int socket_unit_add_connection(t_socket_unit_s *su, socket_t socket);
ret_t socket_unit_select_loop(t_socket_unit_s *su);

#endif /* H_SOCKET_UNIT_H */
