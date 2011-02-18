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
#include "utils.h"

t_socket srv_sock;

int main(const int argc, const char *argv[])
{
    int done = 0;
    fprintf(stdout, "starting\n");
    if (    (config_manager_loadfile(DEFAULT_CONFIGFILE)!=MMP_ERR_OK) ||
            (mmp_socket_initSystem()!=MMP_ERR_OK) ||
            (module_loader_load(config_get())!=MMP_ERR_OK) ||
            (mmp_socket_server_start(config_get()->server->listen_port,
                                        config_get()->server->listen_queue,
                                        &srv_sock)!=MMP_ERR_OK) )
        goto bad_exit;
    mmp_trace_reset();
    while(!done) {
        t_socket newsock;
        t_socket_unit_s *su;
        char *nip;
        if ((su = xmalloc(sizeof(*su)))==NULL) {
            DBG_PRINT(("enomem\n"));
            goto badexit;
        }
        socket_unit_init(su);
        if (mmp_socket_server_accept(&srv_sock, &su->socket, &nip)!=MMP_ERR_OK)
            goto bad_exit;
        mmp_socket_set_nonblocking(&su->socket);
        DBG_PRINT(("main loop: %s connected\n", nip));
        request_parse_read(su);
        xfree(nip);
    }
    mmp_socket_close(&srv_sock, 1);
    mmp_socket_finiSystem();
    config_manager_freeall();
    return EXIT_SUCCESS;

bad_exit:
    mmp_trace_print(stdout);
    return EXIT_FAILURE;
}

