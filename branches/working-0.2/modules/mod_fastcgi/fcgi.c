#include "fcgi.h"

static int get_procdata(const char *fname, struct fcgi_app *ap)
{
    
}

int fcgi_run(struct request_s *req, int sock)
{
    logmsg(LOG_INFO, "fcgi run %s", req->abs_filename);

}

