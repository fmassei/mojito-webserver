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
#include <mmp/mmp_queue.h>
#include "socket_unit.h"
#include "config_manager.h"
#include "request_parse.h"
#include "defaults.h"
#include "module_loader.h"
#include "scheduler.h"
#include "lptask.h"
#include "logger.h"
#include "utils.h"

#define SCHEDULER_LEN   10000 /* TODO move to config */
#define LPTASKSEC       4

/* very very static global stuff.. */
static t_socket s_srv_sock;                         /* listener socket */
static t_sched_id s_sched_id;                       /* scheduler id */
static t_socket_unit_s s_sockunits[SCHEDULER_LEN];  /* socket units */
/* options from command line */
static char *s_conffile = NULL;                     /* custom config file */

static t_mmp_queue_s *s_ka_queue;

ret_t kaqueue_create(int size)
{
    if ((s_ka_queue = mmp_queue_create(size))==NULL) {
        mmp_setError_ext(MMP_ERR_GENERIC, "error creating kaqueue.");
        return MMP_ERR_GENERIC;
    }
    return MMP_ERR_OK;
}
void kaqueue_destroy(void)
{
    mmp_queue_destroy(&s_ka_queue);
}

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
    socket_unit_init(&s_sockunits[cl_sock], 0);
    strcpy(s_sockunits[cl_sock].req.IPaddr, inet_ntoa(addr.sin_addr));
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
    if (s_sockunits[sock].state!=SOCKET_STATE_KEEPALIVE)
        log_hit(&s_sockunits[sock].req, &s_sockunits[sock].res);
    scheduler_del_socket(s_sched_id, sock);
    socket_unit_drop(&s_sockunits[sock], 0);
    (void)mmp_socket_close(&sock, shut);
}

static void keep_alive(t_socket sock)
{
    const t_config_s *cfg = config_get();
    log_hit(&s_sockunits[sock].req, &s_sockunits[sock].res);
    DBG_PRINT(("keeping alive slot %d\n", sock));
    socket_unit_drop(&s_sockunits[sock], 1);
    socket_unit_init(&s_sockunits[sock], 1);
    s_sockunits[sock].state = SOCKET_STATE_KEEPALIVE;
    s_sockunits[sock].req.keeping_alive_killtime = 
            time(NULL) + cfg->server->keepalive_timeout;
    if (s_sockunits[sock].req.keeping_alive_hits==-1) {
        s_sockunits[sock].req.keeping_alive_hits = cfg->server->keepalive_max;
        if (mmp_queue_enqueue(s_ka_queue, &s_sockunits[sock])!=MMP_ERR_OK) {
            /* error. try to free the queue and retry */
            lptask();
            if (mmp_queue_enqueue(s_ka_queue, &s_sockunits[sock])!=MMP_ERR_OK)
                /* whatever. just die. */
                kill_client(sock, 1);
        }
    }
    if ((--s_sockunits[sock].req.keeping_alive_hits)<=0) {
        DBG_PRINT(("max keepalive hits for slot %d\n", sock));
        kill_client(sock, 1);
    }
}

void lptask(void)
{
    t_socket_unit_s *eptr;
    DBG_PRINT(("lptask running.\n"));
    lptask_update_timer();
    while (mmp_queue_first(s_ka_queue, &eptr)!=MMP_ERR_EMPTY) {
        if (eptr->req.keeping_alive_killtime>time(NULL))
            break;
        DBG_PRINT(("ka timeout.\n"));
        mmp_queue_dequeue(s_ka_queue, &eptr);
        kill_client(eptr->res.sock, 1);
    }
}
static ret_t client_action(t_socket sock)
{
    t_request_parse_e rreq;
    t_response_send_e rres;
    DBG_PRINT(("data on slot %d\n", sock));
    if (s_sockunits[sock].state==SOCKET_STATE_KEEPALIVE)
        s_sockunits[sock].state = SOCKET_STATE_READREQUEST;
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
                kill_client(sock, 0); /* don't shutdown the socket */
            } else if (rres==RESPONSE_SEND_FINISH) {
                if (s_sockunits[sock].req.keeping_alive)
                    keep_alive(sock);
                else
                    kill_client(sock, 1);
            } else if (rres==RESPONSE_SEND_CONTINUE) {
                s_sockunits[sock].state = SOCKET_STATE_WRITERESPONSE;
            } else {
                /* error */
                kill_client(sock, 1);
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
            if (s_sockunits[sock].req.keeping_alive)
                keep_alive(sock);
            else
                kill_client(sock, 1);
            break;
        case RESPONSE_SEND_MODDONE:
            if (s_sockunits[sock].req.keeping_alive)
                keep_alive(sock);
            else
                kill_client(sock, 0); /* don't shut down the socket */
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

static void usage(void)
{
    printf(PACKAGE " Ver:" VERSION "\n\n");
    printf("options:\n");
    printf("\t-c <filename>   use filename as the config file\n");
    printf("\t-v              print the version\n");
    printf("\t-h              print this help screen\n");
    printf("\n");
    printf("Copyright 2011 Francesco Massei\n");
    printf("Report bugs to <" PACKAGE_BUGREPORT ">\n");
}

static ret_t parse_command_line(const int argc, char * const *argv)
{
    int opt;
    while ((opt = xgetopt(argc, argv, "c:vh"))>=0) {
        switch (opt) {
        case 'c':
            MMP_XFREE_AND_NULL(s_conffile);
            if ((s_conffile = xstrdup(optarg))==NULL) {
                mmp_setError(MMP_ERR_ENOMEM);
                return MMP_ERR_ENOMEM;
            }
            break;
        case 'v':
            printf(PACKAGE" ver:"VERSION"\n");
            exit(EXIT_SUCCESS);
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        default:
            printf("use -h to see the options\n");
            exit(EXIT_FAILURE);
        }
    }
    return MMP_ERR_OK;
}

int main(const int argc, char * const *argv)
{
    int done = 0;
    if ((s_conffile = xstrdup(DEFAULT_CONFIGFILE))==NULL)
        goto bad_exit;
    if (parse_command_line(argc, argv)!=MMP_ERR_OK)
        goto bad_exit;
    printf(PACKAGE" starting\n");
    printf("using config file '%s'\n", s_conffile);
    if (    (config_manager_loadfile(s_conffile)!=MMP_ERR_OK) ||
            (log_init()!=MMP_ERR_OK) ||
            (lptask_init()!=MMP_ERR_OK) ||
            (kaqueue_create(SCHEDULER_LEN)!=MMP_ERR_OK) ||
            (mmp_socket_initSystem()!=MMP_ERR_OK) ||
            (module_loader_load(config_get())!=MMP_ERR_OK) ||
            ((s_sched_id = scheduler_create(SCHEDULER_LEN))<0) ||
            (mmp_socket_server_start_bind(config_get()->server->listen_port,
                                        config_get()->server->listen_queue,
                                        config_get()->server->interface,
                                        &s_srv_sock)!=MMP_ERR_OK) ||
            (scheduler_add_listen_socket(s_sched_id, s_srv_sock)!=MMP_ERR_OK) )
        goto bad_exit;
    log_err(LOGTYPE_INFO, "up and running");
    if (s_conffile!=NULL)
        MMP_XFREE_AND_NULL(s_conffile);
    mmp_trace_reset();
    while(!done) {
        if (scheduler_loop(s_sched_id, someone_calling,
                    lptask_ms2run(), lptask)!=SCHEDRET_OK) {
            mmp_trace_print(stdout);
            goto bad_exit;
        }
    }
    mmp_socket_close(&s_srv_sock, 1);
    scheduler_destroy(s_sched_id);
    mmp_socket_finiSystem();
    log_fini();
    config_manager_freeall();
    return EXIT_SUCCESS;

bad_exit:
    /* bad exit (on fatal error). We can rely on the logger, as log_err simply
     * writes on stderr in any case. */
    log_err(LOGTYPE_FATAL, "fatal error!\n");
    if (s_conffile!=NULL)
        MMP_XFREE_AND_NULL(s_conffile);
    mmp_trace_print(stdout);
    return EXIT_FAILURE;
}

