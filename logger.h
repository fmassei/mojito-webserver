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

#ifndef H_LOGGER_H
#define H_LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include "fparams.h"

/* debug messages are always a pain. As we're stick to C89 we have no
 * variable-length argument lists for macros. We use our ol' double-parenthesis
 * trick based on macro expansion. */
#ifndef NDEBUG
#define DEBUG_LOG(args) logmsg args
#else
#define DEBUG_LOG(args)
#endif

void logmsg(int prio, char *fmt, ...);
void logflush();
void loghit(char *in_ip, char *method_str, char *uri);
int logger_set_params(struct fparam_s *pars);
int logger_init();
int logger_fini();

#endif /* H_LOGGER_H */
