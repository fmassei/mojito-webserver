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
#ifndef H_LOGGER_H
#define H_LOGGER_H

#include "utils.h"
#include <mmp/mmp_error.h>
#include "request.h"
#include "response.h"

typedef enum logtype_e {
    LOGTYPE_INFO,
    LOGTYPE_WARN,
    LOGTYPE_ERR,
    LOGTYPE_FATAL,
} t_logtype_e;

ret_t log_init(void);
void log_hit(t_request_s *req, t_response_s *res);
void log_err(t_logtype_e type, const char *fmt, ...);
void log_fini(void);

#endif /* H_LOGGER_H */
