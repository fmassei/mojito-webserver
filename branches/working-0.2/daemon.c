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

#include "daemon.h"

/* temporary signal handler */
static void _tmp_hndl(int s)
{
    switch(s) {
    case SIGALRM: exit(EXIT_FAILURE); break;
    case SIGUSR1: exit(EXIT_SUCCESS); break;
    case SIGCHLD: exit(EXIT_FAILURE); break;
    }
}

/* save the pid file */
static int save_pid(fparams_st *params)
{
    int lfp = -1;
    char str[10];
    if (params->pidfile==NULL || params->pidfile[0]=='\0') {
        return -1;
    }
    if ((lfp = open(params->pidfile, O_RDWR | O_CREAT, 0640))<0) {
        syslog(LOG_ERR, "unable to create pid file %s, code=%d (%s)",
                params->pidfile, errno, strerror(errno));
        return -1;
    }
    if (lockf(lfp, F_TLOCK, 0)<0) {
        syslog(LOG_ERR, "unable to lock pid file %s, code=%d (%s)",
                params->pidfile, errno, strerror(errno));
        return -1;
    }
    sprintf(str, "%d\n", getpid());
    write(lfp, str, strlen(str));
    return 0;
}

/* change running permissions */
static int change_permissions(fparams_st *params)
{
    if (setuid(params->uid)<0 || setuid(params->gid)<0) {
        syslog(LOG_ERR, "unable to set running permissions, code=%d (%s)",
                errno, strerror(errno));
        return -1;
    }
    return 0;
}

static int clean_fork()
{
    pid_t pid;
    signal(SIGCHLD, _tmp_hndl);
    signal(SIGUSR1, _tmp_hndl);
    signal(SIGALRM, _tmp_hndl);
    pid = fork();
    if (pid<0) {
        syslog(LOG_ERR, "unable to fork daemon, code=%d (%s)",
                errno, strerror(errno));
        return -1;
    }
    if (pid>0) {
        alarm(2);
        pause();
        return -1;
    }
    return 0;
}

static int setsignals(void(*termfunc)(int))
{
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGTERM, termfunc);
    return 0;
}

/* do all the job of forking */
int fork_to_background(fparams_st *params, void(*termfunc)(int))
{
    pid_t sid, parent;
    if (getppid()==1)
        return 0;
    if (change_permissions(params)<0) return -1;
    if (clean_fork()<0) return -1;
    if (save_pid(params)<0) return -1;
    parent = getppid();
    if (setsignals(termfunc)<0) return -1;
    umask(027); /* mask to 0755 */
    if ((sid = setsid())<0) {
        syslog(LOG_ERR, "unable to create a new session, code=%d (%s)",
                errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if ((chdir(params->http_root))<0) {
        syslog(LOG_ERR, "unable to change directory to %s, code=%d (%s)",
                "/", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
    kill(parent, SIGUSR1);
    return 0;
}

