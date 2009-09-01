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

int _compress(unsigned char *addr, int fd, ssize_t len)
{
    if (write(fd, addr, len)!=len)
        return -1;
    return 0;
}

ssize_t _prelen(struct stat *sb)
{
    return sb->st_size;
}

#ifdef MODULE_STATIC
struct module_filter_s *identity_getmodule()
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
    p->compress = _compress;
    p->prelen = _prelen;
    return p;
}

