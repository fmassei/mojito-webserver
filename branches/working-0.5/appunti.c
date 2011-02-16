#include <aio.h>

struct aiocb aiost;

bzero(&aiost, sizeof(aiost));
aiost.aio_fildes = fd;
aiost.aio_buf = buf;
aiost.aio_nbytes = bufsize;
aiost.aio_offset = 0;

aiost.aio_sigevent.sigev_notify = SIGEV_CALLBACK;
aiost.aio_sigevent.notify_function = aio_complete_hndl;
aiost.aio_sigevent.notify_attributes = NULL;
aiost.aio_sigevent.sigev_value.sival_ptr = &aiost;

ret = aio_read(&aiost);

void aio_complete_hndl(sigval_t sigval)
{
    struct aiocb *aiost;
    aiost = (struct aiocb *)sigval.sival_ptr;
    if (aio_error(aiost)==0) {
        ret = aio_return(aiost);
    }
}

