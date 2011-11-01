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
#include <time.h>
#include "lptask.h"
#include "config_manager.h"

static time_t s_time4lptask;

void lptask_update_timer(void)
{
    s_time4lptask = time(NULL)+config_get()->server->lptask_sec;
}

ret_t lptask_init(void)
{
    lptask_update_timer();
    return MMP_ERR_OK;
}

int lptask_ms2run(void)
{
    int secs;
    secs = s_time4lptask-time(NULL);
    return (secs>0)?secs*1000:0;
}

