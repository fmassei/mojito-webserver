#ifndef H_SOCKET_UNIT_MANAGER_H
#define H_SOCKET_UNIT_MANAGER_H

#include <mmp/mmp_thread.h>
#include "socket_unit.h"
#include "config_manager.h"

ret_t socket_unit_management_start(t_sckunit_fptr data_cback);
t_socket_unit_s *socket_unit_management_getsu(void);
ret_t socket_unit_management_stop(void);

#endif /* H_SOCKET_UNIT_MANAGER_H */
