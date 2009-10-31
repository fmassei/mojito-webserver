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

#ifdef DYNAMIC
#include <dlfcn.h>
#endif
#include "../logger.h"
#include "../request.h"
#include "../resp_headers.h"
#include "../plist.h"

/* module functions return codes */
#define MOD_NOHOOK      -4      /* no registered hook for this module */
#define MOD_CORE_CRIT   -3      /* drop everything *don't use if possible* this
                                * must be used when the module mess with the
                                * core so much that, first it surrenders, then
                                * it informs the core that it has to die too. */
#define MOD_CRIT        -2      /* module critical state: the module will be
                                * unloaded */
#define MOD_ERR         -1      /* error */
#define MOD_OK          0       /* ok */
#define MOD_PROCDONE    1       /* don't run further modules for this hook */
#define MOD_ALLDONE     2       /* break the request *don't use if possible* 
                                * this will inform the core that all the further
                                * steps will be accomplished by the module, and
                                * so the core won't call them. */

/* module categories */
#define MODCAT_UNSPEC   0
#define MODCAT_FILTER   1
#define MODCAT_CACHE    2
#define MODCAT_DYNAMIC  3

struct module_s {
    char *name;
    int (*set_params)(struct plist_s *);
    int (*init)(void);
    int (*fini)(void);
    int (*can_run)(struct request_s *);
    int (*on_accept)(void);
    int (*on_presend)(int, struct request_s *);
    int (*on_prehead)(struct stat *);
    int (*on_send)(void *, int, struct stat *);
    int (*on_postsend)(struct request_s *, char *, void *, struct stat *);
    int will_run;
    int category;
    struct module_s *next, *prev;
};

struct module_filter_s {
    struct module_s *mod;
    struct module_filter_s *next;
};

/* wrappers */
int mod_set_params(struct plist_s *params);
int mod_init(void);
int mod_fini(void);
int can_run(struct request_s *req);
int on_accept(void);
int on_presend(int sock, struct request_s *req);
int on_prehead(struct stat *sb);
int on_send(void *addr, int sock, struct stat *sb);
int on_postsend(struct request_s *, char *mime, void *addr, struct stat *sb);
/* loaders */
struct module_s *module_add_static(struct module_s *(*get_module)(void));
#ifdef DYNAMIC
struct module_s *module_add_dynamic(char *fname, char **error);
#endif /* DYNAMIC */

#endif /* H_MODULES_H */
