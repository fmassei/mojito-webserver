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
#include "config_manager.h"
#include "request_parse.h"
#include "defaults.h"
#include "module_loader.h"
#include "scheduler.h"
#include "utils.h"

#define SCHEDULER_LEN   10000 /* TODO move to config */

t_socket srv_sock;
t_sched_id sched_id;
t_socket_unit_s su[SCHEDULER_LEN];

void someone_calling(t_socket sock)
{
    if (sock==srv_sock) {
        t_socket cl_sock;
        char *nip;
        if (mmp_socket_server_accept(&srv_sock, &cl_sock, &nip)!=MMP_ERR_OK) {
            DBG_PRINT(("WHOAH! error on accept.\n"));
            mmp_trace_print(stdout);
            exit(EXIT_FAILURE);
        }
        if (cl_sock>=SCHEDULER_LEN) { /* TODO remove */
            DBG_PRINT(("socket_unit overbound.. C'mon man, fix that!\n"));
            mmp_trace_print(stdout);
            exit(EXIT_FAILURE);
        }
        socket_unit_init(&su[cl_sock]);
        su[cl_sock].socket = cl_sock;
        mmp_socket_set_nonblocking(&cl_sock);
        DBG_PRINT(("main loop: %s connected\n", nip));
        scheduler_add_client_socket(sched_id, cl_sock);
        xfree(nip);
    } else {
        t_request_parse_e rst;
        DBG_PRINT(("data on slot %d\n", sock));
        if (su[sock].state==SOCKET_STATE_READREQUEST) {
            rst = request_parse_read(&su[sock]);
            switch(rst) {
            case REQUEST_PARSE_ERROR:
            case REQUEST_PARSE_CLOSECONN:
kill_connection:
                DBG_PRINT(("closing on slot %d\n", sock));
                (void)mmp_socket_close(&sock, 1);
                scheduler_del_socket(sched_id, sock);
                break;
            case REQUEST_PARSE_FINISH:
                DBG_PRINT(("finished parsing on slot %d\n", sock));
                su[sock].state = SOCKET_STATE_WRITERESPONSE;
                response_send(&su[sock]);
                goto kill_connection;
                break;
            case REQUEST_PARSE_CONTINUE:
                DBG_PRINT(("continue parsing on slot %d\n", sock));
                break;
            } 
        }
    }
}

int main(/*const int argc, const char *argv[]*/)
{
    int done = 0;
    fprintf(stdout, "starting\n");
    if (    (config_manager_loadfile(DEFAULT_CONFIGFILE)!=MMP_ERR_OK) ||
            (mmp_socket_initSystem()!=MMP_ERR_OK) ||
            (module_loader_load(config_get())!=MMP_ERR_OK) ||
            ((sched_id = scheduler_create(SCHEDULER_LEN))<0) ||
            (mmp_socket_server_start(config_get()->server->listen_port,
                                        config_get()->server->listen_queue,
                                        &srv_sock)!=MMP_ERR_OK) ||
            (scheduler_add_listen_socket(sched_id, srv_sock)!=MMP_ERR_OK) )
        goto bad_exit;
    mmp_trace_reset();
    while(!done) {
        if (scheduler_loop(sched_id, someone_calling)!=MMP_ERR_OK) {
            DBG_PRINT(("scheduler_loop\n"));
            goto bad_exit;
        }
    }
    mmp_socket_close(&srv_sock, 1);
    scheduler_destroy(sched_id);
    mmp_socket_finiSystem();
    config_manager_freeall();
    return EXIT_SUCCESS;

bad_exit:
    mmp_trace_print(stdout);
    return EXIT_FAILURE;
}

