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

static char *logfile, *errfile;
FILE *flog, *ferr;

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
static int _logger_set_params(struct plist_s *pars)
{
    params = pars;
    if ((logfile = plist_search(params, "logfile"))==NULL)
        return -1;
    if ((errfile = plist_search(params, "errfile"))==NULL)
        return -1;
    return 0;
}

/* init */
static int _logger_init(void)
{
    if ((flog = fopen(logfile, "w+"))==NULL) {
        fprintf(stderr, "Logger: Error opening file %s\n", logfile);
        return -1;
    }
    if ((ferr = fopen(errfile, "w+"))==NULL) {
        fprintf(stderr, "Logger: Error opening file %s\n", errfile);
        fclose(flog);
        return -1;
    }
    return 0;
}

/* fini */
static int _logger_fini(void)
{
    int ret = 0;
    ret |= fclose(flog);
    ret |= fclose(ferr);
    return !(ret==0);
}

/* format an "hit" log entry */
static void _loghit(char *in_ip, char *method_str, char *uri)
{
    fprintf(flog, "%s - - [%s] \"%s %s\"\n", in_ip, outdate(), method_str, uri);
}

/* log a generic message, on a specific file */
static void _f_logmsg(int prio, char *fmt, va_list argp)
{
    fprintf(ferr, "[%s] [%s %d] ", outdate(), errs[prio], getpid());
    vfprintf(ferr, fmt, argp);
    fprintf(ferr, "\n");
}

/* flush the logger stream */
static void _logflush()
{
    fflush(flog);
    fflush(ferr);
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
    p->base.module_init = _logger_init;
    p->base.module_fini = _logger_fini;
    p->base.module_set_params = _logger_set_params;
    p->loghit = _loghit;
    p->f_logmsg = _f_logmsg;
    p->logflush = _logflush;
    return p;
}

