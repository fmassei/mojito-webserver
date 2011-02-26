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

static t_module_list_s *s_modules = NULL;
static t_module_list_s *s_filters = NULL;

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

/* FIXME horrible style, but I didn't find a better way... */
#define MOD_LOOP_HEAD \
    t_module_s *p; \
    t_module_ret_e ret; \
    t_mmp_listelem_s *el; \
    for (el=s_modules->head; el!=NULL; el=el->next) { \
        p = (t_module_s*)(el->data);
#define MOD_LOOP_SKIP_STOPPED \
        if (p->will_run==0) \
            continue;
#define MOD_LOOP_SWITCH \
        switch(ret) { \
        case MOD_NOHOOK: \
            break; \
        case MOD_CORE_CRIT: \
            return MODRET_ERR;
#define MOD_LOOP_CASE_CRIT \
        case MOD_CRIT: \
            dropmod(s_modules, &p); \
            continue;
#define MOD_LOOP_CASE_ERR \
        case MOD_ERR: \
            p->will_run = 0; \
            break;
#define MOD_LOOP_CASE_PROCDONE \
        case MOD_PROCDONE: \
            return MODRET_OK;
#define MOD_LOOP_CASE_ALLDONE \
        case MOD_ALLDONE: \
            return MODRET_ALLDONE;
#define MOD_LOOP_CASE_CONTINUE \
        case MOD_AGAIN: \
            return MODRET_CONTINUE;
#define MOD_LOOP_END \
        default: \
            break; \
        } \
    }

#define MOD_LOOP_NORMFLOW \
    MOD_LOOP_SKIP_STOPPED \
    MOD_LOOP_SWITCH \
    MOD_LOOP_CASE_CONTINUE \
    MOD_LOOP_CASE_PROCDONE \
    MOD_LOOP_CASE_ALLDONE \
    MOD_LOOP_CASE_CRIT \
    MOD_LOOP_CASE_ERR \
    MOD_LOOP_END
 
t_modret_e mod_set_params(t_config_module_s *params)
{
    MOD_LOOP_HEAD
        ret = (p->set_params!=NULL) ? p->set_params(params) : MOD_NOHOOK;
    MOD_LOOP_SWITCH
    MOD_LOOP_CASE_CRIT
    MOD_LOOP_CASE_ERR
    MOD_LOOP_END
    return MODRET_OK;
}

t_modret_e mod_init(void)
{
    MOD_LOOP_HEAD
        ret = (p->init!=NULL) ? p->init() : MOD_NOHOOK;
    MOD_LOOP_SWITCH
    MOD_LOOP_CASE_CRIT
    MOD_LOOP_CASE_ERR
    MOD_LOOP_END
    return MODRET_OK;
}

t_modret_e mod_fini(void)
{
    MOD_LOOP_HEAD
        ret = (p->fini!=NULL) ? p->fini() : MOD_NOHOOK;
    MOD_LOOP_SWITCH
    MOD_LOOP_CASE_CRIT
    MOD_LOOP_CASE_ERR
    MOD_LOOP_END
    return MODRET_OK;
}

t_modret_e can_run(t_request_s *req)
{
    MOD_LOOP_HEAD
        ret = (p->can_run!=NULL) ? p->can_run(req) : MOD_NOHOOK;
        if (ret==MOD_OK)
            p->will_run = 1;
    MOD_LOOP_NORMFLOW
    return MODRET_OK;
}

t_modret_e on_accept(void)
{
    MOD_LOOP_HEAD
        ret = (p->on_accept!=NULL) ? p->on_accept() : MOD_NOHOOK;
    MOD_LOOP_NORMFLOW
    return MODRET_OK;
}

t_modret_e on_presend(t_socket sock, t_request_s *req)
{
    MOD_LOOP_HEAD
        ret = (p->on_presend!=NULL) ? p->on_presend(sock, req) : MOD_NOHOOK;
    MOD_LOOP_NORMFLOW
    return MODRET_OK;
}

t_modret_e on_prehead(t_response_s *res)
{
    MOD_LOOP_HEAD
        if ((p->category==MODCAT_FILTER) && (p!=res->ch_filter)) {
            ret = MOD_NOHOOK;
        } else {
            ret = (p->on_prehead!=NULL) ? p->on_prehead(res) : MOD_NOHOOK;
        }
    MOD_LOOP_NORMFLOW
    return MODRET_OK;
}

t_modret_e on_send(t_response_s *res)
{
    MOD_LOOP_HEAD
        if ((p->category==MODCAT_FILTER) && (p!=res->ch_filter)) {
            ret = MOD_NOHOOK;
        } else {
            ret = (p->on_send!=NULL) ? p->on_send(res) : MOD_NOHOOK;
        }
    MOD_LOOP_NORMFLOW
    return MODRET_OK;
}

t_modret_e on_postsend(t_request_s *req, t_response_s *res)
{
    MOD_LOOP_HEAD
        ret = (p->on_postsend!=NULL) ? p->on_postsend(req, res) : MOD_NOHOOK;
    MOD_LOOP_NORMFLOW
    return MODRET_OK;
}

/* loaders */
t_module_s *module_add_static(t_get_module_f get_module)
{
    t_module_s *p;
    if ((p = get_module())==NULL)
        return NULL;
    if (s_modules==NULL) {
        if ((s_modules = mmp_list_create())==NULL) {
            mmp_setError(MMP_ERR_GENERIC);
            return NULL;
        }
    }
    if (mmp_list_add_data(s_modules, p)!=MMP_ERR_OK) {
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
    }
    return p;
}

#ifndef DISABLE_DYNAMIC
t_module_s *module_add_dynamic(char *fname)
{
    t_get_module_f get_module;
    void (*fptr)(void);
    if ((fptr = mmp_dl_open_and_get_fnc(fname, "getmodule"))==NULL)
        return NULL;
#ifdef _WIN32
#   pragma warning(push)
#   pragma warning(disable:4055)   /* disable cast warning */
#endif
    get_module = (t_get_module_f)fptr;
#ifdef _WIN32
#   pragma warning(pop)
#endif
    return module_add_static(get_module);
}
#endif /* DISABLE_DYNAMIC */

t_module_list_s *module_getfilterlist(void)
{
    return s_filters;
}

