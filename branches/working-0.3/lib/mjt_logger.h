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

#ifndef H_MJT_LOGGER_H
#define H_MJT_LOGGER_H

#include <mjt_types.h>

/* FIXME: add a compile switch to use syslog! right now syslog.h is used only
 * to get the LOG_* defines */
#if HAVE_SYSLOG_H
#   include <syslog.h>
#endif

#ifndef NDEBUG
#   define DEBUG_LOG(args) mjt_logmsg args
#else
#   define DEBUG_LOG(args)
#endif

BEGIN_C_DECLS

extern void mjt_logmsg(int_t prio, char_t *fmt, ...);
extern int_t mjt_logger_init(char_t *logfile, char_t *errfile);
extern int_t mjt_logger_fini(void);
extern void mjt_logger_flush(void);

END_C_DECLS

#endif /* H_MJT_DATE_H */
