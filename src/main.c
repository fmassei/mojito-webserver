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
#include <mmp/mmp_getopt.h>
#include "socket_unit.h"
#include "config_manager.h"
#include "request_parse.h"
#include "defaults.h"
#include "module_loader.h"
#include "scheduler.h"
#include "utils.h"

#define SCHEDULER_LEN   10000 /* TODO move to config */

static t_socket s_srv_sock;
static t_sched_id s_sched_id;
static t_socket_unit_s s_sockunits[SCHEDULER_LEN];

/* TODO: wrap the accept calls */
static ret_t accept_client(void)
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    t_socket cl_sock;
#ifdef HAVE_ACCEPT4
    cl_sock = accept4(s_srv_sock, (struct sockaddr *)&addr, &addrlen,
                        SOCK_NONBLOCK);
#else
    cl_sock = accept(s_srv_sock, (struct sockaddr *)&addr, &addrlen);
#endif
    if (cl_sock<0) {
        if (errno==EMFILE) {
            DBG_PRINT(("fd limit reached. :(\n"));
            return MMP_ERR_OK;
        }
        perror("accept");
        return MMP_ERR_SOCKET;
    }
    if (cl_sock>=SCHEDULER_LEN) { /* TODO remove */
        DBG_PRINT(("socket_unit overbound.. C'mon man, fix that!\n"));
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    socket_unit_init(&s_sockunits[cl_sock]);
    s_sockunits[cl_sock].socket = cl_sock;
#ifndef HAVE_ACCEPT4 /* already done with accept4() */
    mmp_socket_set_nonblocking(&cl_sock);
#endif
    scheduler_add_client_socket(s_sched_id, cl_sock);
    DBG_PRINT(("main loop: accepted %s\n", inet_ntoa(addr.sin_addr)));
    return MMP_ERR_OK;
}

static void kill_client(t_socket sock, int shut)
{
    if (shut)
        DBG_PRINT(("shutting slot %d\n", sock));
    else
        DBG_PRINT(("closing on slot %d\n", sock));
    (void)mmp_socket_close(&sock, shut);
    scheduler_del_socket(s_sched_id, sock);
}

static ret_t client_action(t_socket sock)
{
    t_request_parse_e rreq;
    t_response_send_e rres;
    DBG_PRINT(("data on slot %d\n", sock));
    if (s_sockunits[sock].state==SOCKET_STATE_READREQUEST) {
        rreq = request_parse_read(&s_sockunits[sock]);
        switch(rreq) {
        case REQUEST_PARSE_ERROR:
        case REQUEST_PARSE_CLOSECONN:
            kill_client(sock, 1);
            break;
        case REQUEST_PARSE_FINISH:
            DBG_PRINT(("finished parsing on slot %d\n", sock));
            rres = response_send(&s_sockunits[sock]);
            if (rres==RESPONSE_SEND_MODDONE) {
                /* done by module */
                DBG_PRINT(("done by module\n"));
                kill_client(sock, 0);
            } else if (rres!=RESPONSE_SEND_CONTINUE) {
                /* error, or it's already finished */
                kill_client(sock, 1);
            } else {
                s_sockunits[sock].state = SOCKET_STATE_WRITERESPONSE;
            }
            break;
        case REQUEST_PARSE_CONTINUE:
            DBG_PRINT(("continue parsing on slot %d\n", sock));
            break;
        } 
    } else if (s_sockunits[sock].state==SOCKET_STATE_WRITERESPONSE) {
        rres = response_send(&s_sockunits[sock]);
        switch(rres) {
        case RESPONSE_SEND_CLOSECONN:
        case RESPONSE_SEND_ERROR:
        case RESPONSE_SEND_FINISH:
            kill_client(sock, 1);
            break;
        case RESPONSE_SEND_MODDONE:
            kill_client(sock, 0);
            break;
        case RESPONSE_SEND_CONTINUE:
            DBG_PRINT(("continue sending on slot %d\n", sock));
            break;
        }
    }
    return MMP_ERR_OK;
}

static t_schedfnc_ret_e someone_calling(t_socket sock)
{
    if (sock==s_srv_sock) {
        if (accept_client()!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return SCHEDRET_CBACKERR;
        }
    } else {
        if (client_action(sock)!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return SCHEDRET_CBACKERR;
        }
    }
    return SCHEDFNCRET_OK;
}

int main(const int argc, char * const *argv)
{
    char *conffile = NULL;
    int done = 0, opt;
    fprintf(stdout, "starting\n");
    if ((conffile = xstrdup(DEFAULT_CONFIGFILE))==NULL)
        goto bad_exit;
    while ((opt = xgetopt(argc, argv, "c:"))>=0) {
        switch (opt) {
        case 'c':
            MMP_XFREE_AND_NULL(conffile);
            if ((conffile = xstrdup(optarg))==NULL) {
                mmp_setError(MMP_ERR_GENERIC);
                goto bad_exit;
            }
            break;
        default:
            printf("unknown option.\n");
            return EXIT_FAILURE;
        }
    }
    printf("Using config file '%s'\n", conffile);
    if (    (config_manager_loadfile(conffile)!=MMP_ERR_OK) ||
            (mmp_socket_initSystem()!=MMP_ERR_OK) ||
            (module_loader_load(config_get())!=MMP_ERR_OK) ||
            ((s_sched_id = scheduler_create(SCHEDULER_LEN))<0) ||
            (mmp_socket_server_start_bind(config_get()->server->listen_port,
                                        config_get()->server->listen_queue,
                                        config_get()->server->interface,
                                        &s_srv_sock)!=MMP_ERR_OK) ||
            (scheduler_add_listen_socket(s_sched_id, s_srv_sock)!=MMP_ERR_OK) )
        goto bad_exit;
    if (conffile!=NULL) MMP_XFREE_AND_NULL(conffile);
    mmp_trace_reset();
    while(!done) {
        if (scheduler_loop(s_sched_id, someone_calling)!=SCHEDRET_OK) {
            mmp_trace_print(stdout);
            goto bad_exit;
        }
    }
    mmp_socket_close(&s_srv_sock, 1);
    scheduler_destroy(s_sched_id);
    mmp_socket_finiSystem();
    config_manager_freeall();
    return EXIT_SUCCESS;

bad_exit:
    printf("Fatal error!\n");
    if (conffile!=NULL) MMP_XFREE_AND_NULL(conffile);
    mmp_trace_print(stdout);
    return EXIT_FAILURE;
}

