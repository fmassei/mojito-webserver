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
#include "env_setter.h"
#include <mmp/mmp_memory.h>
#include <mmp/mmp_string.h>

ret_t env_add(char **nuenv, int nenv, const char *entry)
{
    if ((nuenv = xrealloc(nuenv, (nenv+1)*sizeof(char*)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    if (entry!=NULL) {
        if ((nuenv[nenv++] = xstrdup(entry))==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            return MMP_ERR_ENOMEM;
        }
    } else {
        nuenv[nenv++] = (char*)0;
    }
    return MMP_ERR_OK;
}

ret_t env_add_kv(char **nuenv, int nenv, const char *key, const char *val)
{
    char *p;
    if ((p = xmalloc(strlen(key)+strlen(val)+2))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    sprintf(p, "%s=%s", key, val);
    if (env_add(nuenv, nenv, p)!=0) {
        mmp_setError(MMP_ERR_ENOMEM);
        xfree(p);
        return MMP_ERR_ENOMEM;
    }
    xfree(p);
    return MMP_ERR_OK;
}

ret_t env_add_kl(char **nuenv, int nenv, const char *key, long val)
{
    char *p;
    if ((p = xmalloc(strlen(key)+15+2))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    sprintf(p, "%s=%lu", key, val);
    if (env_add(nuenv, nenv, p)!=0) {
        xfree(p);
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    xfree(p);
    return 0;
}

