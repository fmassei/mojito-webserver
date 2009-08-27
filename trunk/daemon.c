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

#define LOGFILE_OMODE (O_RDWR | O_APPEND | O_CREAT)
#define LOGFILE_MODE 0640

/* save the pid file */
static int save_pid(fparams_st *params)
{
    int lfp;
    char str[10];
    if (params->pidfile==NULL) {
        logmsg(LOG_ERROR, "No pidfile specified");
        return -1;
    }
    if ((lfp = open(params->pidfile, O_RDWR | O_CREAT, 0640))<0) {
        logmsg(LOG_ERROR, "Error opening pid file: %s", strerror(errno));
        return -1;
    }
    if (lockf(lfp, F_TLOCK, 0)<0) {
        logmsg(LOG_ERROR, "Error locking pid file: %s", strerror(errno));
        return -1;
    }
    sprintf(str, "%d\n", getpid());
    write(lfp, str, strlen(str));
    return 0;
}

/* that's tricky. If we reopen the stdout and stderr on a non-valid file we
 * don't know where to send the error :-) So let's check the validity of the
 * files before */
static int precheck_stds(fparams_st *params)
{
    int fd;
    if (params->logfile!=NULL) {
        if ((fd = open(params->logfile, LOGFILE_OMODE, LOGFILE_MODE))<0) {
            logmsg(LOG_ERROR, "Error opening log file: %s", strerror(errno));
            return -1;
        }
        close(fd);
    }
    if (params->errfile!=NULL) {
        if ((fd = open(params->errfile, LOGFILE_OMODE, LOGFILE_MODE))<0) {
            logmsg(LOG_ERROR, "Error opening err file: %s", strerror(errno));
            return -1;
        }
        close(fd);
    }
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
    if (params->logfile!=NULL) {
        if (open(params->logfile, LOGFILE_OMODE, LOGFILE_MODE)!=1)
            return -1;
    } else {
        if (dup(0)!=1)
            return -1;
    }
    if (params->errfile!=NULL) {
        if (open(params->errfile, LOGFILE_OMODE, LOGFILE_MODE)!=2)
            return -1;
    } else {
        if (dup(1)!=2)
            return -1;
    }
    return 0;
}

/* set the webserver root folder */
static int change_root(fparams_st *params)
{
    if (params->http_root==NULL) {
        logmsg(LOG_ERROR, "No http root specified");
        return -1;
    }
    if (chdir(params->http_root)!=0) {
        logmsg(LOG_ERROR, "Error chdiring into directory: %s", strerror(errno));
        return -1;
    }
    /* XXX: chroot is disabled because of too many problems everywhere, mainly
     * on interpreted cgi (that will be chrooted too). Anyway,
     * if the user wants to chroot mojito, he will do it the standard way */
/*    if (chroot(params->http_root)!=0) {
        logmsg(LOG_ERROR, "Error chrooting directory: %s", strerror(errno));
        return -1;
    }*/
    if (setuid(params->uid)<0) {
        logmsg(LOG_ERROR, "Error setting uid: %s", strerror(errno));
        return -1;
    }
    if (setuid(params->gid)<0) {
        logmsg(LOG_ERROR, "Error setting gid: %s", strerror(errno));
        return -1;
    }
    return 0;
}

/* do all the job of forking */
int fork_to_background(fparams_st *params, void(*termfunc)(int))
{
    pid_t i;
    if (getppid()==1) return 0;/* already a daemon */
    if (precheck_stds(params)<0) return -1;
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

