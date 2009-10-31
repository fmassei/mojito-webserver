#include "fcgi.h"

extern struct la_s *la;

static int start_new(struct fcgi_app *slot, const char *fname)
{
    int pid;
    char *nuenv = NULL;
    strcpy(slot->fname, fname);
    slot->reqId = 1;
    pid = fork();
    if (pid<0)
        return -1;
    if (pid!=0)
        return pid;
    if (dup2(slot->fsock[1], FCGI_LISTENSOCK_FILENO)==-1) {
        slot->fname[0] = '\0';
        return -1;
    }
    execle(fname, basename((char*)fname), (char*)0, nuenv);
    perror("execl");
    return -1;
}

static struct fcgi_app *get_procdata(const char *fname)
{
    struct fcgi_app *p;
    int i;
    for (i=0; i<POOL_SIZE; ++i) {
        p = ((struct fcgi_app *)laget(la))+i;
        if (p->fname[0]=='\0') {
            if (start_new(p, fname)<0)
                return NULL;
        } else {
            if (!strcmp(fname, p->fname)) {
                return p;
            }
        }
    }
    return NULL;
}

int fcgi_run(struct request_s *req, int sock)
{
    struct fcgi_app *p;
    logmsg(LOG_INFO, "fcgi run %s", req->abs_filename);
    if ((p = get_procdata(req->abs_filename))==NULL)
        return -1;
    return 0;
    UNUSED(sock);
}

