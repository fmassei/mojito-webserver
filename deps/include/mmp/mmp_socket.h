#ifndef H_MMP_SOCKET_H
#define H_MMP_SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef _WIN32
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <sys/time.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <fcntl.h>
#   include <unistd.h>
    typedef int socket_t;
#   define  SOCKET_INVALID  (-1)
#else
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   include <winsock2.h>
#   include <windows.h>
#   include <ws2tcpip.h>
    typedef SOCKET socket_t;
#   define SOCKET_INVALID   INVALID_SOCKET
#endif

#include "mmp_trace.h"
#include "mmp_memory.h"
#include "mmp_string.h"

ret_t socket_initSystem(void);
ret_t socket_finiSystem(void);
ret_t socket_server_start(int port, int qsize, socket_t *sock);
ret_t socket_close(socket_t *sock, int shut);
ret_t socket_server_accept(socket_t *listen_sock, socket_t *out, char **ip);
int socket_server_select(int nfds, fd_set *rd, fd_set *wd, fd_set *ex,
                                                            struct timeval *to);

#endif /* H_MMP_SOCKET_H */
