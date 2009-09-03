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

/* save the pid file */
static int save_pid(fparams_st *params)
{
    int lfp;
    char str[10];
    if (params->pidfile==NULL) {
        fprintf(stderr, "No pidfile specified");
        return -1;
    }
    if ((lfp = open(params->pidfile, O_RDWR | O_CREAT, 0640))<0) {
        fprintf(stderr, "Error opening pid file: %s", strerror(errno));
        return -1;
    }
    if (lockf(lfp, F_TLOCK, 0)<0) {
        fprintf(stderr, "Error locking pid file: %s", strerror(errno));
        return -1;
    }
    sprintf(str, "%d\n", getpid());
    write(lfp, str, strlen(str));
    return 0;
}


/* that function is a pain: no logging possible and too much things that can
 * go wrong. */
static int reopen_stds(fparams_st *params)
{
    int i;
    for (i=getdtablesize(); i>=0; --i)
        close(i);
    if (open("/dev/null", O_RDWR)!=0) return -1;
    if (dup(0)!=1) return -1;
    if (dup(0)!=2) return -1;
    return 0;
}

/* set the webserver root folder */
static int change_root(fparams_st *params)
{
    if (params->http_root==NULL) {
        fprintf(stderr, "No http root specified");
        return -1;
    }
    if (chdir(params->http_root)!=0) {
        fprintf(stderr, "Error chdiring into directory: %s", strerror(errno));
        return -1;
    }
    /* XXX: chroot is disabled because of too many problems everywhere, mainly
     * on interpreted cgi (that will be chrooted too). Anyway,
     * if the user wants to chroot mojito, he will do it the standard way */
/*    if (chroot(params->http_root)!=0) {
        fprintf(stderr, "Error chrooting directory: %s", strerror(errno));
        return -1;
    }*/
    if (setuid(params->uid)<0) {
        fprintf(stderr, "Error setting uid: %s", strerror(errno));
        return -1;
    }
    if (setuid(params->gid)<0) {
        fprintf(stderr, "Error setting gid: %s", strerror(errno));
        return -1;
    }
    return 0;
}

/* do all the job of forking */
int fork_to_background(fparams_st *params, void(*termfunc)(int))
{
    pid_t i;
    if (getppid()==1) return 0;/* already a daemon */
    i = fork();
    if (i<0) return -1; /* fork error */
    if (i>0) exit(0); /* parent exit */
    if (setsid()==-1) return -1; /* new process group */
    if (reopen_stds(params)!=0) return -1;
    umask(027); /* mask to 0755 */
    if (save_pid(params)!=0) return -1;
    if (change_root(params)!=0) return -1;
    signal(SIGCHLD, SIG_IGN); /* ignore child */
    signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTERM, termfunc); /* terminating signal */
    return 0;
}

