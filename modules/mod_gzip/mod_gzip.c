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

#include "../modules.h"
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

#define CHUNK 16384

static int _compress(unsigned char *addr, int fd, ssize_t len)
{
    int ret;
    ssize_t have;
    z_stream strm;
    unsigned char out[CHUNK];
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    if ((ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION,
                    Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY))!=Z_OK)
        return ret;
    strm.avail_in = len;
    strm.next_in = addr;
    do {
        strm.avail_out = CHUNK;
        strm.next_out = out;
        deflate(&strm, Z_FINISH);
        have = CHUNK - strm.avail_out;
        if (write(fd, out, have)!=have) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
    } while (strm.avail_out==0);
    (void)deflateEnd(&strm);
    return Z_OK;
}

static ssize_t _prelen(struct stat *sb)
{
    if (sb->st_size==0)
        return 0;
    return -1;
}

static int _can_run(struct request_s *req)
{
    struct qhead_s *p;
    for (p=req->header.accept_encoding; p!=NULL; p=p->next)
        if (!strcmp("gzip", p->id))
            return MOD_OK;
    return MOD_ERR;
}

static int _on_prehead(struct stat *sb)
{
    ssize_t len;
    if ((len = _prelen(sb))>=0)
        header_push_contentlength(len);
    header_push_contentencoding("gzip");
    return MOD_PROCDONE;
}

static int _on_send(void *addr, int sock, struct stat *sb)
{
    if (_compress(addr, sock, sb->st_size)!=Z_OK)
        return MOD_CRIT;
    return MOD_PROCDONE;
}

#ifdef MODULE_STATIC
struct module_s *mod_gzip_getmodule()
#else
struct module_s *getmodule()
#endif
{
    struct module_s *p;
    if ((p = malloc(sizeof(*p)))==NULL)
        return NULL;
    p->name = "gzip";
    p->set_params = NULL;
    p->init = NULL;
    p->fini = NULL;
    p->can_run = _can_run;
    p->on_accept = NULL;
    p->on_presend = NULL;
    p->on_prehead = _on_prehead;
    p->on_send = _on_send;
    p->on_postsend = NULL;
    p->next = p->prev = NULL;
    p->will_run = 1;
    p->category = MODCAT_FILTER;
    return p;
}

