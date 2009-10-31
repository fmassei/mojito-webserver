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
#ifndef H_FASTCGI_H
#define H_FASTCGI_H

#include "proto_fcgi.h"
#include "linear_array.h"
#include "../modutils.h"
#include "../../defines.h"
#include "../../request.h"

/* number of pre-created socket pairs (aka max runnable fcgi apps) */
#define POOL_SIZE    10

struct fcgi_app {
    int fsock[2];
    int reqId;
    char fname[256];
};

int fcgi_run(struct request_s *req, int sock);

#endif /* H_FASTCGI_H */
