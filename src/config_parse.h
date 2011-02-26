/*
    Copyright 2010 Francesco Massei

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
#ifndef H_CONFIG_PARSE_H
#define H_CONFIG_PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mmp/mmp_list.h>
#include <mmp/mmp_string.h>
#include <mmp/mmp_memory.h>
#include <disml/disml.h>
#include "config_type.h"

/* destroy a config object */
void config_destroy(t_config_s **config);
/* get a config object from a disobj object */
t_config_s *disobj_to_config(t_disobj_s *obj);

#endif /* H_CONFIG_PARSE_H */
