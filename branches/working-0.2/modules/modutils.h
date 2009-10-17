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

#ifndef H_MODUTILS_H
#define H_MODUTILS_H

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int add_env(char **nuenv, int nenv, char *entry);
int add_env_kv(char **nuenv, int nenv, char *key, char *val);
int add_env_kl(char **nuenv, int nenv, char *key, long val);

#endif /* H_MODUTILS_H */
