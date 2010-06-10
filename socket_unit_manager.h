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
#ifndef H_SOCKET_UNIT_MANAGER_H
#define H_SOCKET_UNIT_MANAGER_H

#include <mmp/mmp_thread.h>
#include "socket_unit.h"
#include "config_manager.h"

ret_t socket_unit_management_start(t_sckunit_fptr data_cback);
t_socket_unit_s *socket_unit_management_getsu(void);
ret_t socket_unit_management_stop(void);

#endif /* H_SOCKET_UNIT_MANAGER_H */
