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
#ifndef H_RESP_HEADERS_H
#define H_RESP_HEADERS_H

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "fparams.h"
#include "request.h"
#include "date.h"

#define HRESP_200   0
#define HRESP_404   1
#define HRESP_406   2
#define HRESP_411   3
#define HRESP_500   4
#define HRESP_501   5

void header_push_code(int code);

void header_push_contentlength(long len);
void header_push_contenttype(char *name);
void header_push_contentencoding(char *name);

void header_part_send(int sock);
void header_send(int sock);

void header_kill_w_code(int code, int sock);

#endif /* H_RESP_HEADERS_H */
