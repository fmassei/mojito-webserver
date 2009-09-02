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

#include "../filter.h"
#include <zlib.h>

#define CHUNK 16384

int _compress(unsigned char *addr, int fd, ssize_t len)
{
    int ret;
    ssize_t have;
    z_stream strm;
    unsigned char out[CHUNK];
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    if ((ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION))!=Z_OK)
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

ssize_t _prelen(struct stat *sb)
{
    if (sb->st_size==0)
        return 0;
    return -1;
}

#ifdef MODULE_STATIC
struct module_filter_s *deflate_getmodule()
#else
struct module_filter_s *getmodule()
#endif
{
    struct module_filter_s *p;
    if ((p = malloc(sizeof(*p)))==NULL)
        return NULL;
    p->base.module_init = NULL;
    p->base.module_fini = NULL;
    p->base.module_set_params = NULL;
    p->name = strdup("deflate");
    p->compress = _compress;
    p->prelen = _prelen;
    return p;
}

