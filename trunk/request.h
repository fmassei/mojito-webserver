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

#ifndef H_REQUEST_H
#define H_REQUEST_H

/* We need snprintf() and mkstemp() */
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include "fileutils.h"
#include "fparams.h"
#include "header_w_quality.h"
#include "logger.h"

/* FIXME change to system defined size */
#define SOCKBUFSIZE 255

/* parser constants */
#define HEAD    1
#define CR1     2
#define LF1     3
#define CR2     4
#define BODY    5

/* method defines */
#define M_GET   1
#define M_HEAD  2
#define M_POST  3

/* protocol */
#define P_HTTP_UNK  0   
#define P_HTTP_10   1
#define P_HTTP_11   2

/* TODO FIXME XXX move all the other fields in here! */
struct request_s {
    char *in_ip;

    char *uri;
    int method;
    char *method_str;   /* pointer to method in string form */

    char *content_type;
    long content_length;
    struct header_s {
        struct qhead_s *accept_encoding;
    } header;

    /* the divided filename/querystring pointers */
    char *page, *qs;
    int post_fd;
    /* the decoded filename on the filesystem (if any) */
    char *abs_filename;
    /* file stats */
    struct stat sb;
};

/* create an empty request */
void request_create();
/* wait some data on a living connection */
int request_waitonalive(int sock);
/* parse the request */
int request_read(int sock);

#endif /* H_REQUEST_H */
