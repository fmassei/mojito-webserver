#include "request.h"

static void request_parse_init(t_request_parse_s *rp)
{
    rp->buf = rp->cur_head = NULL;
    rp->status = REQUEST_PARSE_STATUS_HEAD;
}

t_request_s *request_create(void)
{
    t_request_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    request_parse_init(&ret->parse);
    return ret;
}

void request_destroy(t_request_s **request)
{
    if (request==NULL || *request==NULL) return;
    xfree(*request);
    *request = NULL;
}

