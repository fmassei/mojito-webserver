#include <disml/disml.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_socket.h>
#include "config_manager.h"
#include "defaults.h"

socket_t srv_sock;

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
    while(!done) {
        
    }
    socket_close(&srv_sock, 1);
    socket_finiSystem();
    config_manager_freeall();
    return EXIT_SUCCESS;
}

