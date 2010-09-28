/*
    Copyright 2010 Francesco Massei

    This file is part of mojito webserver.

        Mojito is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Mojito is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Mojito.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <disml/disml.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_socket.h>
#include <mmp/mmp_thread.h>
#include "socket_unit.h"
#include "socket_unit_manager.h"
#include "config_manager.h"
#include "request_parse.h"
#include "defaults.h"
#include "module_loader.h"

t_socket srv_sock;

ret_t sck_data(int slot, t_socket_unit_s *su)
{
    t_request_parse_e rst;
    if (su->socket_states[slot]==SOCKET_STATE_READREQUEST) {
keep_request_alive:
        rst = request_parse_read(&su->connect_list[slot], su->reqs[slot]);
        switch (rst) {
        case REQUEST_PARSE_ERROR:
            /* TODO: mark and close */
        case REQUEST_PARSE_CLOSECONN:
kill_connection:
            /* TODO: check for these errors! */
            (void)mmp_socket_close(&su->connect_list[slot], 1);
            (void)socket_unit_del_connection(su, slot);
            printf("disconnected\n");
            break;
        case REQUEST_PARSE_FINISH:
            su->socket_states[slot]==SOCKET_STATE_WRITERESPONSE;
            response_send(su->resps[slot], su->reqs[slot]);
            /*if (su->reqs[slot]->keeping_alive) {
                printf("keeping alive\n");
                goto keep_request_alive;
            }*/
            goto kill_connection;
            break;
        case REQUEST_PARSE_CONTINUE:
            /* nothing */
            break;
        }
    }
    return MMP_ERR_OK;
}

#ifndef UNIT_TESTING
int main(/*const int argc, const char *argv[]*/)
{
    int done = 0;
    fprintf(stdout, "starting\n");
    if (config_manager_loadfile(DEFAULT_CONFIGFILE)!=MMP_ERR_OK) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    if (mmp_socket_initSystem()!=MMP_ERR_OK) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    if (module_loader_load(config_get())!=MMP_ERR_OK) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    if (mmp_socket_server_start(config_get()->server->listen_port,
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
        t_socket newsock;
        t_socket_unit_s *sock_unit;
        char *nip;
        if (mmp_socket_server_accept(&srv_sock, &newsock, &nip)!=MMP_ERR_OK) {
            mmp_trace_print(stdout);
            return EXIT_FAILURE;
        }
        mmp_socket_set_nonblocking(&newsock);
        if ((sock_unit = socket_unit_management_getsu())==NULL) {
            mmp_trace_print(stdout);
            return EXIT_FAILURE;
        }
        printf("%s connected\n", nip);
        socket_unit_add_connection(sock_unit, newsock);
        xfree(nip);
    }
    socket_unit_management_stop();
    mmp_socket_close(&srv_sock, 1);
    mmp_socket_finiSystem();
    config_manager_freeall();
    return EXIT_SUCCESS;
}
#endif /* UNIT_TESTING */

