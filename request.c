#include "request.h"

t_request_s *request_create(void)
{
    t_request_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    return ret;
}

void request_destroy(t_request_s **request)
{
    if (request==NULL || *request==NULL) return;
    xfree(*request);
    *request = NULL;
}

