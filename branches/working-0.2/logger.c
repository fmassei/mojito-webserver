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

#include "logger.h"

static char datestr[30];

static char *logfile, *errfile;
FILE *flog, *ferr;

/* get the string representing the passed prio */
static char *get_logprio_desc(int prio)
{
    switch(prio) {
    case LOG_EMERG: return "emerg";
    case LOG_ALERT: return "alert";
    case LOG_CRIT: return "crit";
    case LOG_ERR: return "err";
    case LOG_WARNING: return "warning";
    case LOG_NOTICE: return "notice";
    case LOG_INFO: return "info";
    case LOG_DEBUG: return "debug";
    default: return "";
    }
}

/* return a date formatted for logs */
static char *outdate()
{
    time_t t;
    t = time(NULL);
    strftime(datestr, sizeof(datestr), "%a %b %d %H:%M:%S %Y", localtime(&t));
    return datestr;
}

/* log a generic message, on a specific file */
static void _f_logmsg(int prio, char *fmt, va_list argp)
{
    fprintf(ferr, "[%s] [%s %d] ", outdate(), get_logprio_desc(prio), getpid());
    vfprintf(ferr, fmt, argp);
    fprintf(ferr, "\n");
}

void logmsg(int prio, char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    _f_logmsg(prio, fmt, argp);
    va_end(argp);
}

/* get the module parameters */
int logger_set_params(fparams_st *params)
{
    logfile = params->logfile;
    errfile = params->errfile;
    return 0;
}

/* init */
int logger_init(void)
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
int logger_fini(void)
{
    int ret = 0;
    ret |= fclose(flog);
    ret |= fclose(ferr);
    return !(ret==0);
}

/* format an "hit" log entry */
void loghit(char *in_ip, char *method_str, char *uri)
{
    fprintf(flog, "%s - - [%s] \"%s %s\"\n", in_ip, outdate(), method_str, uri);
}

/* flush the logger stream */
void logflush()
{
    fflush(flog);
    fflush(ferr);
}

