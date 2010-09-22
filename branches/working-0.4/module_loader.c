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
#include "module_loader.h"

#ifndef DISABLE_DYNAMIC
ret_t module_loader_load(t_config_s *params)
{
    t_mmp_listelem_s *p;
    t_module_s *mod;
    t_config_module_s *mod_conf;
    if (params==NULL || params->modules==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    for (p=params->modules->head; p!=NULL; p=p->next) {
        mod_conf = (t_config_module_s*)p->data;
        if (mod_conf==NULL || mod_conf->name==NULL)
            continue;
        mod = module_add_dynamic(mod_conf->name);
        if (mod==NULL || mod->set_params==NULL)
            continue;
        mod->set_params(mod_conf);
    }
    return MMP_ERR_OK;
}
#endif
