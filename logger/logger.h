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
#include "../fparams.h"
#include "../module.h"

#define LOG_ERROR   0
#define LOG_WARN    1
#define LOG_INFO    2
#define LOG_DEBUG   3

/* debug messages are always a pain. As we're stick to C89 we have no
 * variable-length argument lists for macros. We use our ol' double-parenthesis
 * trick based on macro expansion. */
#ifndef NDEBUG
#define DEBUG_LOG(args) logmsg args
#else
#define DEBUG_LOG(args)
#endif

struct module_logger_s {
    /* init/fini/params */
    struct module_fnc_s base;
    /* TODO to be removed. get the global parameters */
    void (*logger_set_global_parameters)(fparams_st *);
    /* log a message */
    void (*f_logmsg)(int, char *, va_list);
    /* flush the logger stream */
    void (*logflush)(void);
    /* log when a page was hit */
    void (*loghit)(char *, char *, char *);
};

void logger_set_global_parameters(fparams_st *pars);
void logmsg(int prio, char *fmt, ...);
void logflush();
void loghit(char *in_ip, char *method_str, char *uri);

int logger_add_static_mod(struct module_logger_s*(*get_module)(void));
int logger_add_dynamic_mod(char *fname, char **error);

#endif /* H_LOGGER_H */
