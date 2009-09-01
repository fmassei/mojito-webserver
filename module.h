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

#ifndef H_MODULE_H
#define H_MODULE_H

#include "plist.h"

/* basic module functions */
struct module_fnc_s {
    /* init/fini */
    int(*module_init)(void);
    int(*module_fini)(void);
    /* set module parameters */
    int(*module_set_params)(struct plist_s *);
};

int module_get_logger(fparams_st *prm);
int module_get_cache(fparams_st *prm);

#endif /* H_MODULE_H */
