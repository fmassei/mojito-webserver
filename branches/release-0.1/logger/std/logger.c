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

#include "../logger.h"

static char datestr[30];
static char *errs[] = { "error", "warning", "info", "debug" };

static fparams_st *global_params;

/* return a date formatted for logs */
static char *outdate()
{
    time_t t;
    t = time(NULL);
    strftime(datestr, sizeof(datestr), "%a %b %d %H:%M:%S %Y", localtime(&t));
    return datestr;
}

/* get the global parameters */
void logger_set_global_parameters(fparams_st *pars)
{
    global_params = pars;
}

/* format an "hit" log entry */
void loghit(char *in_ip, char *method_str, char *uri)
{
    printf("%s - - [%s] \"%s %s\"\n", in_ip, outdate(), method_str, uri);
}

/* log a generic message, on a specific file */
void _f_logmsg(int prio, char *fmt, va_list argp)
{
    FILE *f = stderr;
    fprintf(f, "[%s] [%s %d] ", outdate(), errs[prio], getpid());
    vfprintf(f, fmt, argp);
    fprintf(f, "\n");
}

/* log a generic message */
void logmsg(int prio, char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    _f_logmsg(prio, fmt, argp);
    va_end(argp);
}

/* flush the logger stream */
void logflush()
{
    fflush(stdout);
    fflush(stderr);
}

