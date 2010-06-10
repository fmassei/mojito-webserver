#include <disml/disml.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_socket.h>
#include <mmp/mmp_thread.h>
#include "socket_unit.h"
#include "socket_unit_manager.h"
#include "config_manager.h"
#include "request_parse.h"
#include "defaults.h"

socket_t srv_sock;

ret_t sck_data(int slot, t_socket_unit_s *su)
{
    t_request_parse_e rst;
    if (su->socket_states[slot]==SOCKET_STATE_READREQUEST) {
        rst = request_parse_read(&su->connect_list[slot], su->reqs[slot]);
        switch (rst) {
        case REQUEST_PARSE_ERROR:
            /* TODO: mark and close */
        case REQUEST_PARSE_CLOSECONN:
            /* TODO: check for these errors! */
            (void)socket_close(&su->connect_list[slot], 1);
            (void)socket_unit_del_connection(su, slot);
            printf("disconnected\n");
            break;
        case REQUEST_PARSE_FINISH:
            /* TODO: respond! */
            break;
        case REQUEST_PARSE_CONTINUE:
            /* nothing */
            break;
        }
    }
    return MMP_ERR_OK;
}

int main(/*const int argc, const char *argv[]*/)
{
    int done = 0;
    fprintf(stdout, "starting\n");
    if (config_manager_loadfile(DEFAULT_CONFIGFILE)!=MMP_ERR_OK) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    if (socket_initSystem()!=MMP_ERR_OK) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    if (socket_server_start(config_get()->server->listen_port,
                            config_get()->server->listen_queue,
                            &srv_sock)!=MMP_ERR_OK) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    if (socket_unit_management_start(sck_data)!=MMP_ERR_OK) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    while(!done) {
        socket_t newsock;
        t_socket_unit_s *sock_unit;
        char *nip;
        if (socket_server_accept(&srv_sock, &newsock, &nip)!=MMP_ERR_OK) {
            mmp_trace_print(stdout);
            return EXIT_FAILURE;
        }
        /* TODO: set the new socket as non blocking */
        if ((sock_unit = socket_unit_management_getsu())==NULL) {
            mmp_trace_print(stdout);
            return EXIT_FAILURE;
        }
        printf("%s connected\n", nip);
        socket_unit_add_connection(sock_unit, newsock);
        xfree(nip);
    }
    socket_unit_management_stop();
    socket_close(&srv_sock, 1);
    socket_finiSystem();
    config_manager_freeall();
    return EXIT_SUCCESS;
}

