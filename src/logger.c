/*
    Copyright 2011 Francesco Massei

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
#include <stdio.h>
#include <stdarg.h>
#include <mmp/mmp_date.h>
#include "config_manager.h"
#include "logger.h"

static const char *logtype2str(t_logtype_e type)
{
    switch(type) {
    case LOGTYPE_INFO: return "info";
    case LOGTYPE_WARN: return "warning";
    case LOGTYPE_ERR: return "error";
    case LOGTYPE_FATAL: return "fatal";
    }
    return "";
}

ret_t log_init(void)
{
    const t_config_s *cfg = config_get();
    if (cfg->server->log_on_file) {
        if ((freopen(cfg->server->log_file, "w+", stdout)==NULL) ||
            (freopen(cfg->server->err_file, "w+", stderr)==NULL) ) {
            mmp_setError(MMP_ERR_FILE);
            return MMP_ERR_FILE;
        }
    }
    return MMP_ERR_OK;
}

static void outdate(char *datestr, size_t datestr_len)
{
    time_t t;
    t = time(NULL);
    strftime(datestr, datestr_len, "%a %b %d %H:%M:%S %Y", localtime(&t));
}

/* log in combined log format */
void log_hit(t_request_s *req, t_response_s *res)
{
    char datestr[30];
    outdate(datestr, sizeof(datestr));
    fprintf(stdout, "%s - - [%s] \"%s\" %d %lu",
        req->IPaddr, datestr,
        req->first_line,
        hresp2int(res->final_code),
        (unsigned long)res->final_data_sent);
    if (req->referer!=NULL)
        fprintf(stdout, " \"%s\"", req->referer);
    else fprintf(stdout, " -");
    if (req->user_agent!=NULL)
        fprintf(stdout, " \"%s\"", req->user_agent);
    else fprintf(stdout, " -");
    fprintf(stdout, "\n");
#ifndef NDEBUG
    fflush(stdout);
#endif
}

void vlog_err(t_logtype_e type, const char *fmt, va_list argp)
{
    char datestr[30];
    outdate(datestr, sizeof(datestr));
    fprintf(stderr, "[%s] [%s] ", datestr, logtype2str(type));
    vfprintf(stderr, fmt, argp);
    fprintf(stderr, "\n");
#ifndef NDEBUG
    fflush(stderr);
#endif
}
void log_err(t_logtype_e type, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    vlog_err(type, fmt, argp);
    va_end(argp);
}

void log_fini(void)
{
    fflush(stdout);
    fflush(stderr);
}

