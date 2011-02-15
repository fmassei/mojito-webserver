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
#include "socket_unit_manager.h"
#include "utils.h"

static t_socket_unit_s **m_su = NULL;
static t_mmp_thread *m_th = NULL;
static t_sckunit_fptr m_dcback = NULL;

static void *su_thread(void *ptr)
{
    t_socket_unit_s *su = (t_socket_unit_s*)ptr;
    DBG_PRINT(("socket_unit_manager: thread started\n"));
    while (socket_unit_select_loop(su)==MMP_ERR_OK)
        ;
    DBG_PRINT(("socket_unit_manager: su_thread error!\n"));
    mmp_trace_print(stdout);
    return NULL;
}

ret_t socket_unit_management_start(t_sckunit_fptr data_cback)
{
    int i, n_su;
    n_su = config_get()->server->num_socket_units;
    m_su = xmalloc(n_su*sizeof(*m_su));
    m_th = xmalloc(n_su*sizeof(*m_th));
    if (m_su == NULL || m_th == NULL) {
        if (m_su!=NULL) xfree(m_su);
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    m_dcback = data_cback;
    for (i=0; i<n_su; ++i)
        m_su[i] = NULL;
    return MMP_ERR_OK;
}

t_socket_unit_s *socket_unit_management_getsu(void)
{
    int i, n_su;
    t_socket_unit_s *su_ptr = NULL;
    n_su = config_get()->server->num_socket_units;
    for (i=0; i<n_su; ++i) {
        if (m_su[i]==NULL) {
            m_su[i] = socket_unit_create(
                                config_get()->server->num_socket_units_fds);
            if (m_su[i]==NULL) {
                mmp_setError(MMP_ERR_GENERIC);
                return NULL;
            }
            m_su[i]->newdata_cback = m_dcback;
            DBG_PRINT(("socket_unit_management_getsu: mmp_thread_create\n"));
            mmp_thread_create(su_thread, m_su[i], &m_th[i]);
            su_ptr = m_su[i];
            break;
        } else {
            if (m_su[i]->nsockets<m_su[i]->queue_size) {
                su_ptr = m_su[i];
                break;
            }
        }
    }
    return su_ptr;
}

ret_t socket_unit_management_stop(void)
{
    return MMP_ERR_OK;
}

