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
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../modules.h"
#include "../../response.h"

#define SHMNAME "/mojito_stat"

typedef unsigned long stat_type_t;
static struct stat_counters_s {
    stat_type_t init,
                fini,
                accept,
                can_run,
                presend,
                prehead,
                send,
                postsend;
} *counters = NULL;

static char *stat_page = "<html><body>init: %10d<br />accept: %10d<br /></body></html>";

static void send_statistics(int sock, struct request_s *req)
{
    int clen;
    char *buf;
    clen = strlen(stat_page)+12+1;
    if ((buf = malloc(clen))==NULL) {
        header_push_code(HRESP_500);
        return;
    }
    sprintf(buf, stat_page, counters->init, counters->accept);
    header_push_code(HRESP_200);
    header_push_contentlength(clen);
    header_push_contenttype("text/html");
    header_send(sock);
    if (req->method==M_HEAD)
        return;
    write(sock, buf, clen);
}

static int _init(void)
{
    size_t len;
    int fd = -1;
    if ((fd = shm_open(SHNAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR))<0)
        goto error;
    len = sizeof(*counters);
    if (ftruncate(fd, len)==-1)
        goto error;
    counters = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (counters==MAP_FAILED)
        goto error;
    counters->init =
        counters->fini =
        counters->accept =
        counters->can_run =
        counters->presend =
        counters->prehead =
        counters->send =
        counters->postsend = 0;
    ++counters->init;
    return MOD_OK;
error:
    if (fd>=0)
        shm_unlink(SHMNAME);
    return MOD_CRIT;
}
static int _fini(void)
{
    ++counters->fini;
    if (counters!=NULL)
        munmap(counters, sizeof(*counters));
    shm_unlink(SHMNAME);
    return MOD_OK;
}
static int _on_accept(void)
{
    ++counters->accept;
    return MOD_OK;
}
static int _can_run(void)
{
    ++counters->can_run;
    return MOD_OK;
}
static int _on_presend(int sock, struct request_s *req)
{
    ++counters->presend;
    if (!strcmp(req->uri, "/statistics")) {
        send_statistics(sock, req);
        return MOD_ALLDONE;
    }
    return MOD_OK;
}
static int _on_prehead(struct stat *st)
{
    ++counters->prehead;
    return MOD_OK;
}
static int _on_send(void *v, int i, struct stat *st)
{
    ++counters->send;
    return MOD_OK;
}
static int _on_postsend(struct request_s *r, char *c, void *v, struct stat *s)
{
    ++counters->postsend;
    return MOD_OK;
}

#ifdef MODULE_STATIC
struct module_s *mod_stat_getmodule()
#else
struct module_s *getmodule()
#endif
{
    struct module_s *p;
    if ((p = malloc(sizeof(*p)))==NULL)
        return NULL;
    p->name = "stat";
    p->set_params = NULL;
    p->init = _init;
    p->fini = _fini;
    p->on_accept = _on_accept;
    p->on_presend = _on_presend;
    p->on_prehead = _on_prehead;
    p->on_send = _on_send;
    p->on_postsend = _on_postsend;
    p->next = p->prev = NULL;
    p->will_run = 1;
    p->category = MODCAT_UNSPEC;
    return p;
}

