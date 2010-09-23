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

#ifndef H_FILTER_MANAGER_H
#define H_FILTER_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include "modules.h"
#include "header_w_quality.h"
#include "types.h"

ret_t filter_sanitize_queue(t_qhead_list_s **qhead);
t_module_s *filter_findfilter(t_qhead_list_s *qlist);

#endif /* H_FILTER_MANAGER_H */
