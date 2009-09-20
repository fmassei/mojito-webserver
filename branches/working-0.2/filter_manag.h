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

#ifndef H_FILTER_MANAG_H
#define H_FILTER_MANAG_H

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
/* #include "filter/filter.h" */
#include "header_w_quality.h"
#include "logger/logger.h"
#include "modules/modules.h"

/* filter functions */
int filter_sanitize_queue(struct qhead_s **qhead);
struct module_s *filter_findfilter(struct qhead_s *qhead);

#endif /* H_FILTER_MANAG_H */
