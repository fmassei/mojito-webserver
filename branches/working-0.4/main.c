#include <disml/disml.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_socket.h>
#include <mmp/mmp_thread.h>
#include "socket_unit.h"
#include "config_manager.h"
#include "defaults.h"

socket_t srv_sock;

void *socket_unit_thr(void *ptr)
{
    t_socket_unit_s *su = (t_socket_unit_s*)ptr;
    while (socket_unit_select_loop(su)==MMP_ERR_OK)
        ;
    return NULL;
}

ret_t sck_data(int i, socket_t sock)
{
    printf("data from %d %d\n", i, sock);
    return MMP_ERR_OK;
}

int main(/*const int argc, const char *argv[]*/)
{
    int done = 0;
    t_socket_unit_s *ts;
    mmp_thread_t tt;
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
    ts = socket_unit_create(10);
    ts->newdata_cback = sck_data;
    mmp_thread_create(socket_unit_thr, ts, &tt);
    while(!done) {
        socket_t newsock;
        char *nip;
        if (socket_server_accept(&srv_sock, &newsock, &nip)!=MMP_ERR_OK) {
            mmp_trace_print(stdout);
            return EXIT_FAILURE;
        }
        printf("%s connected\n", nip);
        socket_unit_add_connection(ts, newsock);
        xfree(nip);
    }
    socket_close(&srv_sock, 1);
    socket_finiSystem();
    config_manager_freeall();
    return EXIT_SUCCESS;
}

