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
#include "../logger/logger.h"
#include "../request.h"
#include "../plist.h"

#define MOD_CRIT        -1
#define MOD_OK          0
#define MOD_ALLDONE     1

struct module_s {
    int (*set_params)(struct plist_s *);
    int (*init)(void);
    int (*fini)(void);
    int (*on_accept)(void);
    int (*on_presend)(int sock, struct request_s *);
    int (*on_postsend)(struct request_s *, struct module_filter_s *,
                                                        char *, void *, size_t);
    struct module_s *next;
};

/* wrappers */
int mod_set_params(struct plist_s *);
int mod_init(void);
int mod_fini(void);
int on_accept(void);
int on_presend(int sock, struct request_s *);
int on_postsend(struct request_s *, struct module_filter_s *,
                                                        char *, void *, size_t);

/* loaders */
struct module_s *module_add_static(struct module_s *(*get_module)(void));
#ifdef DYNAMIC
struct module_s *module_add_dynamic(char *fname, char **error);
#endif /* DYNAMIC */

#endif /* H_MODULES_H */
