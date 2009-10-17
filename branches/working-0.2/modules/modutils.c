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

#include "modutils.h"

/* add a variable to the new environment */
int add_env(char **nuenv, int nenv, char *entry)
{
    if ((nuenv = realloc(nuenv, (nenv+1)*sizeof(char*)))==NULL)
        return -1;
    if (entry!=NULL) {
        if ((nuenv[nenv++] = strdup(entry))==NULL)
            return -1;
    } else
        nuenv[nenv++] = (char*)0;
    return 0;
}

/* add a key-value(as string) pair to the environment */
int add_env_kv(char **nuenv, int nenv, char *key, char *val)
{
    char *p;
    p = malloc(strlen(key)+strlen(val)+2);
    sprintf(p, "%s=%s", key, val);
    if (add_env(nuenv, nenv, p)!=0) {
        free(p);
        return -1;
    }
    free(p);
    return 0;
}

/* add a key-value(as long) to the environment */
int add_env_kl(char **nuenv, int nenv, char *key, long val)
{
    char *p;
    p = malloc(strlen(key)+15+2);
    sprintf(p, "%s=%lu", key, val);
    if (add_env(nuenv, nenv, p)!=0) {
        free(p);
        return -1;
    }
    free(p);
    return 0;
}

