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

#include <stdio.h>
#include <stdlib.h>
#include <mmp/mmp_memory.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_socket.h>
#include "../../common_src/resp_headers.h"
#include "../../src/modules.h"
#include "../../src/utils.h"
#include "cgi_run.h"

#ifdef _WIN32
#   define OUTLINK  __declspec(dllexport)
#else
#   define OUTLINK
#endif

static t_module_ret_e _on_presend(t_request_s *req, t_response_s *res)
{
    if (access(req->abs_filename, X_OK)==0) {
        if (cgi_run(req, res)!=0) {
            DBG_PRINT(("[mod_cgi] cgi_run critical.\n"));
            mmp_trace_print(stdout);
            return MOD_CRIT;
        }
        return MOD_ALLDONE;
    }
    return MOD_OK;
}
#ifdef MOD_CGI_STATIC
t_module_s *mod_cgi_getmodule(void)
#else
OUTLINK t_module_s *getmodule(void)
#endif
{
    t_module_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    ret->name = "cgi";
    ret->set_params = NULL;
    ret->init = NULL;
    ret->fini = NULL;
    ret->can_run = NULL;
    ret->on_accept = NULL;
    ret->on_presend = _on_presend;
    ret->on_prehead = NULL;
    ret->on_send = NULL;
    ret->on_postsend = NULL;
    ret->category = MODCAT_DYNAMIC;
    return ret;
}

