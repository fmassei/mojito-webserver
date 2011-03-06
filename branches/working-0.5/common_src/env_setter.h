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
#ifndef H_ENV_SETTER_H
#define H_ENV_SETTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_error.h>

ret_t env_add(char **nuenv, int nenv, const char *entry);
ret_t env_add_kv(char **nuenv, int nevn, const char *key, const char *val);
ret_t env_add_kl(char **nuenv, int nevn, const char *key, long val);

#endif /* H_ENV_SETTER_H */

