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
#include "modules.h"

static t_module_list_s *s_all_modules = NULL;
static t_module_list_s *s_filters = NULL;
static t_module_list_s *s_unspecs = NULL;

/* delete a module from the queue.
 * NOTE1: the modules will NOT be unloaded!
 * NOTE2: don't change pointer of *todrop, as it will be impossible to manage
 *      module-drops in a cycle! */
static void dropmod(t_module_list_s *list, t_module_s **todrop)
{
    if (list==NULL) return;
    mmp_list_del_elem_by_data(list, todrop);
    /* TODO FIXME XXX try not to unload modules just free()ing them! */
    xfree(*todrop);
    *todrop = NULL;
}
 
t_modret_e can_run(t_request_s *req, t_response_s *res)
{
    t_module_s *p;
    t_module_ret_e ret;
    int i, mx;
    mx = sizeof(res->mods2run)/sizeof(res->mods2run[0]);
    for (i=0; i<mx; ++i) {
        if ((p = res->mods2run[i])==NULL)
            continue;
        ret = (p->can_run!=NULL) ? p->can_run(req) : MOD_NOHOOK;
        switch(ret) {
        case MOD_NOHOOK:
            continue;
        case MOD_CORE_CRIT:
            return MODRET_ERR;
        case MOD_CRIT:
            res->mods2run[i] = NULL;
            dropmod(s_all_modules, &p);
            continue;
        case MOD_ERR:
            res->mods2run[i] = NULL;
            continue;
        case MOD_PROCDONE:
            return MODRET_OK;
        case MOD_ALLDONE:
            return MODRET_ALLDONE;
        case MOD_AGAIN:
            return MODRET_CONTINUE;
        }
    }
    return MODRET_OK;
}

t_modret_e on_accept(t_request_s *req, t_response_s *res)
{
    t_module_s *p;
    t_module_ret_e ret;
    int i, mx;
    mx = sizeof(res->mods2run)/sizeof(res->mods2run[0]);
    for (i=0; i<mx; ++i) {
        if ((p = res->mods2run[i])==NULL)
            continue;
        ret = (p->on_accept!=NULL) ? p->on_accept() : MOD_NOHOOK;
        switch(ret) {
        case MOD_NOHOOK:
            continue;
        case MOD_CORE_CRIT:
            return MODRET_ERR;
        case MOD_CRIT:
            res->mods2run[i] = NULL;
            dropmod(s_all_modules, &p);
            continue;
        case MOD_ERR:
            res->mods2run[i] = NULL;
            continue;
        case MOD_PROCDONE:
            return MODRET_OK;
        case MOD_ALLDONE:
            return MODRET_ALLDONE;
        case MOD_AGAIN:
            return MODRET_CONTINUE;
        }
    }
    return MODRET_OK;
}

t_modret_e on_presend(t_socket sock, t_request_s *req, t_response_s *res)
{
    t_module_s *p;
    t_module_ret_e ret;
    int i, mx;
    mx = sizeof(res->mods2run)/sizeof(res->mods2run[0]);
    for (i=0; i<mx; ++i) {
        if ((p = res->mods2run[i])==NULL)
            continue;
        ret = (p->on_presend!=NULL) ? p->on_presend(sock, req)
                                    : MOD_NOHOOK;
        switch(ret) {
        case MOD_NOHOOK:
            continue;
        case MOD_CORE_CRIT:
            return MODRET_ERR;
        case MOD_CRIT:
            res->mods2run[i] = NULL;
            dropmod(s_all_modules, &p);
            continue;
        case MOD_ERR:
            res->mods2run[i] = NULL;
            continue;
        case MOD_PROCDONE:
            return MODRET_OK;
        case MOD_ALLDONE:
            return MODRET_ALLDONE;
        case MOD_AGAIN:
            return MODRET_CONTINUE;
        }
    }
    return MODRET_OK;
}

t_modret_e on_prehead(t_response_s *res)
{
    t_module_s *p;
    t_module_ret_e ret;
    int i, mx;
    mx = sizeof(res->mods2run)/sizeof(res->mods2run[0]);
    for (i=0; i<mx; ++i) {
        if ((p = res->mods2run[i])==NULL)
            continue;
        ret = (p->on_prehead!=NULL) ? p->on_prehead(res)
                                    : MOD_NOHOOK;
        switch(ret) {
        case MOD_NOHOOK:
            continue;
        case MOD_CORE_CRIT:
            return MODRET_ERR;
        case MOD_CRIT:
            res->mods2run[i] = NULL;
            dropmod(s_all_modules, &p);
            continue;
        case MOD_ERR:
            res->mods2run[i] = NULL;
            continue;
        case MOD_PROCDONE:
            return MODRET_OK;
        case MOD_ALLDONE:
            return MODRET_ALLDONE;
        case MOD_AGAIN:
            return MODRET_CONTINUE;
        }
    }
    return MODRET_OK;
}

t_modret_e filter_on_send(t_module_s *filter, t_response_s *res)
{
    t_module_ret_e ret;
    ret = filter->on_send(res);
    switch(ret) {
    case MOD_NOHOOK:
        return MODRET_OK;
    case MOD_CRIT:
    case MOD_CORE_CRIT:
    case MOD_ERR:
        return MODRET_ERR;
    case MOD_PROCDONE:
        return MODRET_OK;
    case MOD_ALLDONE:
        return MODRET_ALLDONE;
    case MOD_AGAIN:
        return MODRET_CONTINUE;
    }
}

t_modret_e on_postsend(t_request_s *req, t_response_s *res)
{
    t_module_s *p;
    t_module_ret_e ret;
    int i, mx;
    mx = sizeof(res->mods2run)/sizeof(res->mods2run[0]);
    for (i=0; i<mx; ++i) {
        if ((p = res->mods2run[i])==NULL)
            continue;
        ret = (p->on_postsend!=NULL) ? p->on_postsend(req, res)
                                     : MOD_NOHOOK;
        switch(ret) {
        case MOD_NOHOOK:
            continue;
        case MOD_CORE_CRIT:
            return MODRET_ERR;
        case MOD_CRIT:
            res->mods2run[i] = NULL;
            dropmod(s_all_modules, &p);
            continue;
        case MOD_ERR:
            res->mods2run[i] = NULL;
            continue;
        case MOD_PROCDONE:
            return MODRET_OK;
        case MOD_ALLDONE:
            return MODRET_ALLDONE;
        case MOD_AGAIN:
            return MODRET_CONTINUE;
        }
    }
    return MODRET_OK;
}

/* loaders */
t_module_s *module_add_static(t_get_module_f get_module)
{
    t_module_s *p;
    if ((p = get_module())==NULL)
        return NULL;
    if (s_all_modules==NULL) {
        if ((s_all_modules = mmp_list_create())==NULL) {
            mmp_setError(MMP_ERR_GENERIC);
            return NULL;
        }
    }
    if (mmp_list_add_data(s_all_modules, p)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_GENERIC);
        return NULL;
    }
    if (p->category==MODCAT_FILTER) {
        if (s_filters==NULL) {
            if ((s_filters = mmp_list_create())==NULL) {
                mmp_setError(MMP_ERR_GENERIC);
                return NULL;
            }
        }
        if (mmp_list_add_data(s_filters, p)!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return NULL;
        }
    } else if (p->category==MODCAT_UNSPEC) {
        if (s_unspecs==NULL) {
            if ((s_unspecs = mmp_list_create())==NULL) {
                mmp_setError(MMP_ERR_GENERIC);
                return NULL;
            }
        }
        if (mmp_list_add_data(s_unspecs, p)!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return NULL;
        }
    }
    return p;
}

#ifndef DISABLE_DYNAMIC_MODULES
t_module_s *module_add_dynamic(char *fname)
{
    t_get_module_f get_module;
    void (*fptr)(void);
    if ((fptr = mmp_dl_open_and_get_fnc(fname, "getmodule"))==NULL)
        return NULL;
    get_module = (t_get_module_f)fptr;
    return module_add_static(get_module);
}
#endif /* DISABLE_DYNAMIC_MODULES */

t_module_list_s *module_getfilterlist(void)
{
    return s_filters;
}

t_module_list_s *module_getunspecslist(void)
{
    return s_unspecs;
}

ret_t module_fill_response_vector(t_response_s *resp)
{
    t_mmp_listelem_s *el;
    int i, mx;
    mx = sizeof(resp->mods2run)/sizeof(resp->mods2run[0]);
    if (s_unspecs!=NULL) {
        for (i=0, el=s_unspecs->head; el!=NULL && i<mx; el=el->next, ++i) {
            resp->mods2run[i] = (t_module_s*)el->data;
        }
    }
    for (; i<mx; ++i)
        resp->mods2run[i] = NULL;
    return MMP_ERR_OK;
}

