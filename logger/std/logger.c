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

static struct plist_s *params;

/* return a date formatted for logs */
static char *outdate()
{
    time_t t;
    t = time(NULL);
    strftime(datestr, sizeof(datestr), "%a %b %d %H:%M:%S %Y", localtime(&t));
    return datestr;
}

/* get the module parameters */
static void _logger_set_params(struct plist_s *pars)
{
    params = pars;
}

/* format an "hit" log entry */
static void _loghit(char *in_ip, char *method_str, char *uri)
{
    printf("%s - - [%s] \"%s %s\"\n", in_ip, outdate(), method_str, uri);
}

/* log a generic message, on a specific file */
static void _f_logmsg(int prio, char *fmt, va_list argp)
{
    FILE *f = stderr;
    fprintf(f, "[%s] [%s %d] ", outdate(), errs[prio], getpid());
    vfprintf(f, fmt, argp);
    fprintf(f, "\n");
}

/* log a generic message */
static void _logmsg(int prio, char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    _f_logmsg(prio, fmt, argp);
    va_end(argp);
}

/* flush the logger stream */
static void _logflush()
{
    fflush(stdout);
    fflush(stderr);
}

#ifdef MODULE_STATIC
struct module_logger_s *std_getmodule()
#else
struct module_logger_s *getmodule()
#endif
{
    struct module_logger_s *p;
    if ((p = malloc(sizeof(*p)))==NULL)
        return NULL;
    p->base.module_init = NULL;
    p->base.module_fini = NULL;
    p->base.module_set_params = _logger_set_params;
    p->loghit = _loghit;
    p->f_logmsg = _f_logmsg;
    p->logflush = _logflush;
    return p;
}

