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
#ifndef H_RESPONSE_TYPE_H
#define H_RESPONSE_TYPE_H

#include <mmp/mmp_socket.h>
#include "types.h"

typedef enum hresp_e {
    HRESP_200   =   0,
    HRESP_404   =   1,
    HRESP_406   =   2,
    HRESP_500   =   3,
    HRESP_501   =   4
} t_hresp_e;

struct response_s {
    char resbuf[0xff], tmpbuf[0xff];
    t_socket sock;
    t_module_s *ch_filter;      /**< selected filter for response */
    int content_length_sent;    /**< has the content-length been sent? */
};

#endif /* H_RESPONSE_TYPE_H */
