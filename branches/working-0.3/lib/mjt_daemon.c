#include "mjt_daemon.h"

static void _tmp_hndl(int s)
{
    switch(s) {
    case SIGALRM: exit(EXIT_FAILURE); break;
    case SIGUSR1: exit(EXIT_SUCCESS); break;
    case SIGCHLD: exit(EXIT_FAILURE); break;
    }
}

static int_t save_pid(char_t *pidfile)
{
    int_t lfp = -1;
    char_t pidstr[20];
    if (pidfile==NULL || pidfile=='\0')
        return -1;
    if ((lfp = open(pidfile, O_RDWR | O_CREAT, 0640))<0) {
        fprintf(stderr, "unable to create pid file %s, code=%d (%s)",
            pidfile, errno, strerror(errno));
        return -1;
    }
    if (lockf(lfp, F_TLOCK, 0)<0) {
        fprintf(stderr, "unable to lock pid file %s, code=%d (%s)",
            pidfile, errno, strerror(errno));
        return -1;
    }
    sprintf(pidstr, "%d\n", getpid());
    if (mjt_justwrite(lfp, pidstr, strlen(pidstr))==FALSE) {
        fprintf(stderr, "badbadbad error writing pid file %s, code=%d (%s)",
            pidfile, errno, strerror(errno));
        return -1;
    }
    return 0;
}

static int_t change_permissions(uid_t uid, gid_t gid)
{
    if (setuid(uid)<0 || setgid(gid)<0) {
        fprintf(stderr, "unable to set running permissions, code=%d (%s)",
            errno, strerror(errno));
        return -1;
    }
    return 0;
}

static int_t clean_fork()
{
    pid_t pid;
    signal(SIGCHLD, _tmp_hndl);
    signal(SIGUSR1, _tmp_hndl);
    signal(SIGALRM, _tmp_hndl);
    pid = fork();
    if (pid<0) {
        fprintf(stderr, "unable to fork daemon, code=%d (%s)",
            errno, strerror(errno));
    } else if (pid>0) {
        alarm(2);
        pause();
        return -1;
    }
    return 0;
}

static int_t setsignals(void(*termfunc)(int_t))
{
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGTERM, termfunc);
    return 0;
}


int_t fork_to_background(char_t *pidfile, uid_t uid, gid_t gid,
                            char_t *rootdir, void(*termfunc)(int_t))
{
    pid_t sid, parent;
    if (getppid()==1)
        return 0;
    if (change_permissions(uid, gid)<0) return -1;
    if (clean_fork()<0) return -1;
    if (save_pid(pidfile)<0) return -1;
    parent = getppid();
    if (setsignals(termfunc)<0) return -1;
    umask(027); /* mask to 0755 */
    if ((sid = setsid())<0) {
        fprintf(stderr, "unable to create a new session, code=%d (%s)",
            errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (chdir(rootdir)<0) {
        fprintf(stderr, "unable to change directory to %s, code=%d (%s)",
            rootdir, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (    freopen("/dev/null", "r", stdin)==NULL ||
            freopen("/dev/null", "w", stdout)==NULL ||
            freopen("/dev/null", "w", stderr)==NULL ) {
        fprintf(stderr, "unable to re-open the standard streams, code=%d (%s)",
            errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    kill(parent, SIGUSR1);
    return 0;
}

