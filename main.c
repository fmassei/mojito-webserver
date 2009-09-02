/*
    Copyright 2009 Francesco Massei

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

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include "logger/logger.h"
#include "cache/cache.h"
#include "socket.h"
#include "request.h"
#include "fparams.h"
#include "daemon.h"
#include "response.h"
#include "filter.h"
#include "module.h"

extern char *uri;
fparams_st params;
int keeping_alive;

void clean_quit()
{
    cache_fini();
    logger_fini();
}

/* SIGTERM callback */
void sig_term(int signal)
{
    logmsg(LOG_INFO, "caught SIGTERM(%d). Stopping", signal);
    clean_quit();
    exit(0);
}

int main(const int argc, char * const argv[])
{
    int cl_sock, i;
    pid_t cpid;
    int opt;
    char *error;
    extern char *in_ip, *method_str, *uri;

    while ((opt = getopt(argc, argv, "v"))!=-1) {
        switch(opt) {
        case 'v':
            printf("Mojito 0.1\n");
            exit(EXIT_SUCCESS);
            break;
        default:
            fprintf(stderr, "Usage %s [-v]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if ((i = params_loadFromINIFile("config.ini", &params)<0)) {
        if (i==-1)
            perror("Error loading configuration");
        return EXIT_FAILURE;
    }
    if (module_get_logger(&params, &error)<0 ||
            module_get_cache(&params, &error)<0 ||
            module_get_filter(&params, &error)<0 ) {
        fprintf(stderr, "Error loading dynamic modules: %s\n", error);
        return EXIT_FAILURE;
    }
    if (fork_to_background(&params, sig_term)<0) {
        fprintf(stderr, "Forking to background failed.\n");
        return EXIT_FAILURE;
    }
#ifndef NOLOGGER
    /* FIXME adjust the order!! */
    if (logger_init()!=0) {
        fprintf(stderr, "Failed to start logger.\n");
        return EXIT_FAILURE;
    }
#endif
#ifndef NOCACHE
    if (cache_init()!=0) {
        logmsg(LOG_ERROR, "Could not start cache. Trying to go on.");
    }
#endif
    if (server_start(params.listen_port, params.listen_queue)<0) {
        logmsg(LOG_ERROR, "Error starting server");
        return EXIT_FAILURE;
    }
    logmsg(LOG_INFO, "Server started");
    logflush();
    while (1) {
        if ((cl_sock = server_accept())<0) {
            perror("Error accepting connections");
            exit(EXIT_FAILURE);
        }
        keeping_alive = 0;
        cpid = fork();
        if (cpid==-1) {
            perror("Fork() failed!");
            return EXIT_FAILURE;
        }
        if (cpid>0) {
            close(cl_sock);
            continue;
        }
child_life:
        request_create();
        if (request_read(cl_sock)==1)
            goto client_kill;
        send_file(cl_sock, uri);
        loghit(in_ip, method_str, uri);
        if (keeping_alive!=0) {
            DEBUG_LOG((LOG_DEBUG, "Keeping alive!"));
            logflush();
            if (request_waitonalive(cl_sock)>0)
                goto child_life;
        }
client_kill:
        DEBUG_LOG((LOG_DEBUG, "Killing process"));
        shutdown(cl_sock, SHUT_RDWR);
        close(cl_sock);
        exit(0);
    }
    clean_quit();
    return EXIT_FAILURE;    /* this is not the best exit point :) */
}

