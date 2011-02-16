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
#ifndef H_CONFIG_MANAGER_H
#define H_CONFIG_MANAGER_H

#include "config_type.h"
#include "config_parse.h"
#include "defaults.h"

/* get _the_ configuration object. */
const t_config_s * config_get(void);

/* load the configuration from file */
ret_t config_manager_loadfile(const char *filename);
/* free any existing configuration */
void config_manager_freeall(void);

#endif /* H_CONFIG_MANAGER_H */
