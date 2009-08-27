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

#ifndef H_RESPONSE_H
#define H_RESPONSE_H

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "logger/logger.h"
#include "cache/cache.h"
#include "fparams.h"
#include "request.h"
#include "mime.h"
#include "date.h"
#include "compression.h"
#include "filter.h"
#include "cgi.h"
#include "header_w_quality.h"

/* push return codes in the response */
void push_200(int sock);
void send_404(int sock);
void send_500(int sock);
void send_501(int sock);
/* send a file */
void send_file(int sock, const char *filename);

#endif /* H_RESPONSE_H */
