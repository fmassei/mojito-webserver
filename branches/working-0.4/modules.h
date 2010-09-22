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

#ifndef H_MODULES_H
#define H_MODULES_H

#ifndef DISABLE_DYNAMIC
#include <mmp/mmp_dl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <mmp/mmp_trace.h>
#include <mmp/mmp_memory.h>
#include <mmp/mmp_socket.h>
#include <mmp/mmp_files.h>
#include "config_type.h"
#include "request.h"
#include "response.h"
#include "types.h"

/* module functions return codes */
typedef enum module_ret_e {
    MOD_NOHOOK      =   -4,     /* no registered hook for this module */
    MOD_CORE_CRIT   =   -3,     /* drop everything *don't use if possible* this
                                * must be used when the module mess with the
                                * core so much that, first it surrenders, then
                                * it informs the core that it has to die too. */
    MOD_CRIT        =   -2,     /* module critical state: the module will be
                                * unloaded */
    MOD_ERR         =   -1,     /* error */
    MOD_OK          =   0,      /* ok */
    MOD_PROCDONE    =   1,      /* don't run further modules for this hook */
    MOD_ALLDONE     =   2       /* break the request *don't use if possible* 
                                * this will inform the core that all the further
                                * steps will be accomplished by the module, and
                                * so the core won't call them. */
} t_module_ret_e;

/* module categories */
typedef enum module_category_e {
    MODCAT_UNSPEC   =   0,
    MODCAT_FILTER   =   1,
    MODCAT_CACHE    =   2,
    MODCAT_DYNAMIC  =   3
} t_module_category_e;

struct module_s {
    char *name;
    int (*set_params)(t_config_module_s *);
    int (*init)(void);
    int (*fini)(void);
    int (*can_run)(t_request_s *);
    int (*on_accept)(void);
    int (*on_presend)(t_socket, t_request_s *);
    int (*on_prehead)(t_mmp_stat_s *, t_response_s *);
    int (*on_send)(void *, t_mmp_stat_s *, t_response_s *);
    int (*on_postsend)(t_request_s *, char *, void *, t_mmp_stat_s *);
    int will_run;
    int category;
};

/* wrappers */
int mod_set_params(t_config_module_s *params);
int mod_init(void);
int mod_fini(void);
int can_run(t_request_s *req);
int on_accept(void);
int on_presend(t_socket sock, t_request_s *req);
int on_prehead(t_mmp_stat_s *sb, t_response_s *res);
int on_send(void *addr, t_mmp_stat_s *sb, t_response_s *res);
int on_postsend(t_request_s *, char *mime, void *addr, t_mmp_stat_s *sb);

/* loaders */
typedef t_module_s *(*t_get_module_f)(void);

t_module_s *module_add_static(t_get_module_f get_module);
#ifndef DISABLE_DYNAMIC
t_module_s *module_add_dynamic(char *fname);
#endif /* DISABLE_DYNAMIC */

t_module_list_s *module_getfilterlist(void);

#endif /* H_MODULES_H */
