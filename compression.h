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

#ifndef H_COMPRESSION_H
#define H_COMPRESSION_H

#define _BSD_SOURCE
#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "filter.h"

/* filters */
int identity_filter(unsigned char *addr, int fd, ssize_t len);
int identity_filter_prelen(struct stat *sb);
int zlib_filter(unsigned char *addr, int fd, ssize_t len);
int zlib_filter_prelen(struct stat *sb);
int gzip_filter(unsigned char *addr, int fd, ssize_t len);
int gzip_filter_prelen(struct stat *sb);

#endif /* H_COMPRESSION_H */
