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
#include "config_parse.h"

static ret_t copy_or_die_str(char **dest, char *src)
{
    if (dest==NULL || src==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if ((*dest = xstrdup(src))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    return MMP_ERR_OK;
}

static ret_t copy_or_die_int(int *dest, char *src)
{
    char *endptr;
    long tmp;
    if (dest==NULL || src==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    errno = 0;
    tmp = strtol(src, &endptr, 10);
    if ((errno==ERANGE
                && (tmp==LONG_MAX || tmp==LONG_MIN))
            || (errno!=0 && tmp==0) || (endptr==src)
            || (tmp>INT_MAX || tmp<INT_MIN))
        tmp = 0;
    *dest = tmp;
    return MMP_ERR_OK;
}

static void config_module_setting_destroy(t_config_module_setting_s **cms)
{
    if (cms==NULL || *cms==NULL) return;
    if ((*cms)->key!=NULL) xfree((*cms)->key);
    if ((*cms)->val!=NULL) xfree((*cms)->val);
    xfree(*cms);
    *cms = NULL;
}
static void config_module_setting_destroy_v(void **ptr)
{
    config_module_setting_destroy((t_config_module_setting_s**)ptr);
}

static void config_module_destroy(t_config_module_s **cm)
{
    if (cm==NULL || *cm==NULL) return;
    if ((*cm)->name!=NULL) xfree((*cm)->name);
    mmp_list_delete_withdata(&(*cm)->settings, config_module_setting_destroy_v);
    xfree(*cm);
    *cm = NULL;
}
static void config_module_destroy_v(void **ptr)
{
    config_module_destroy((t_config_module_s**)ptr);
}

static void config_server_destroy(t_config_server_s **srv)
{
    if (srv==NULL || *srv==NULL) return;
    if ((*srv)->tmp_dir!=NULL) xfree((*srv)->tmp_dir);
    if ((*srv)->pid_file!=NULL) xfree((*srv)->pid_file);
    if ((*srv)->http_root!=NULL) xfree((*srv)->http_root);
    if ((*srv)->default_page!=NULL) xfree((*srv)->default_page);
    if ((*srv)->log_file!=NULL) xfree((*srv)->log_file);
    if ((*srv)->err_file!=NULL) xfree((*srv)->err_file);
    if ((*srv)->server_meta!=NULL) xfree((*srv)->server_meta);
    if ((*srv)->modules_basepath!=NULL) xfree((*srv)->modules_basepath);
    xfree(*srv);
    *srv = NULL;
}

void config_destroy(t_config_s **config)
{
    if (config==NULL || *config==NULL) return;
    config_server_destroy(&(*config)->server);
    mmp_list_delete_withdata(&(*config)->modules, config_module_destroy_v);
    xfree(*config);
    *config = NULL;
}

static ret_t parse_config_server_kv(t_config_server_s *csrv, t_diskv_s *kv)
{
    char buf[0xff];
    if (csrv==NULL || kv==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if (!strcmp(kv->key, "listen_port"))
        return copy_or_die_int(&(csrv->listen_port), kv->val);
    if (!strcmp(kv->key, "listen_queue"))
        return copy_or_die_int(&(csrv->listen_queue), kv->val);
    if (!strcmp(kv->key, "tmp_dir"))
        return copy_or_die_str(&(csrv->tmp_dir), kv->val);
    if (!strcmp(kv->key, "pid_file"))
        return copy_or_die_str(&(csrv->pid_file), kv->val);
    if (!strcmp(kv->key, "uid"))
        return copy_or_die_int(&(csrv->uid), kv->val);
    if (!strcmp(kv->key, "gid"))
        return copy_or_die_int(&(csrv->gid), kv->val);
    if (!strcmp(kv->key, "keepalive_timeout"))
        return copy_or_die_int(&(csrv->keepalive_timeout), kv->val);
    if (!strcmp(kv->key, "http_root"))
        return copy_or_die_str(&(csrv->http_root), kv->val);
    if (!strcmp(kv->key, "default_page"))
        return copy_or_die_str(&(csrv->default_page), kv->val);
    if (!strcmp(kv->key, "log_file"))
        return copy_or_die_str(&(csrv->log_file), kv->val);
    if (!strcmp(kv->key, "err_file"))
        return copy_or_die_str(&(csrv->err_file), kv->val);
    if (!strcmp(kv->key, "server_meta"))
        return copy_or_die_str(&(csrv->server_meta), kv->val);
    if (!strcmp(kv->key, "modules_basepath"))
        return copy_or_die_str(&(csrv->modules_basepath), kv->val);
    if (!strcmp(kv->key, "num_socket_units"))
        return copy_or_die_int(&(csrv->num_socket_units), kv->val);
    if (!strcmp(kv->key, "num_socket_units_fds"))
        return copy_or_die_int(&(csrv->num_socket_units_fds), kv->val);
    sprintf(buf, "Unknown option %.80s\n", kv->key);
    mmp_setError_ext(MMP_ERR_PARSE, buf);
    return MMP_ERR_PARSE;
}

static t_config_module_setting_s *disobj_to_config_module_setting(
                                                        t_diskv_s *kv)
{
    t_config_module_setting_s *ret;
    if (kv==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return NULL;
    }
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    if (    (copy_or_die_str(&(ret->key), kv->key)!=MMP_ERR_OK) ||
            (copy_or_die_str(&(ret->val), kv->val)!=MMP_ERR_OK) )
        config_module_setting_destroy(&ret);
    return ret;
}

static ret_t parse_config_module_obj(t_mmp_list_s *cm, t_disobj_s *obj)
{
    t_config_module_s *mod;
    t_diselem_s *de;
    t_mmp_listelem_s *p;
    if (cm==NULL || obj==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if ((mod = xmalloc(sizeof(*mod)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    if (    ((mod->name = xstrdup(obj->name))==NULL) ||
            ((mod->settings = mmp_list_create())==NULL) ) {
        config_module_destroy(&mod);
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    for (p=obj->elemlist->head; p!=NULL; p=p->next) {
        if ((de = (t_diselem_s *)p->data)==NULL)
            continue; /* try to go on anyway */
        if (de->type==OT_KV) {
            t_config_module_setting_s *cms;
            cms = disobj_to_config_module_setting(de->elem.kv);
            if (    (cms==NULL) ||
                    (mmp_list_add_data(mod->settings, cms)!=MMP_ERR_OK)   ) {
                config_module_destroy(&mod);
                return MMP_ERR_PARSE;
            }
        } else {
            /* we expect no kv here */
            config_module_destroy(&mod);
            return MMP_ERR_PARSE;
        }
    }
    return mmp_list_add_data(cm, mod);
}

static t_mmp_list_s *disobj_to_config_module(t_disobj_s *obj)
{
    t_mmp_list_s *ret;
    t_diselem_s *de;
    t_mmp_listelem_s *p;
    if (obj==NULL || strcmp(obj->name, "modules")!=0) {
        mmp_setError(MMP_ERR_PARAMS);
        return NULL;
    }
    if ((ret = mmp_list_create())==NULL) {
        mmp_setError(MMP_ERR_GENERIC);
        return NULL;
    }
    for (p=obj->elemlist->head; p!=NULL; p=p->next) {
        if ((de = (t_diselem_s *)p->data)==NULL)
            continue; /* try to go on anyway */
        if (de->type==OT_KV) {
            /* we expect no kv here */
            mmp_list_delete_withdata(&ret, config_module_destroy_v);
            goto done;
        } else {
            if (parse_config_module_obj(ret, de->elem.obj)!=MMP_ERR_OK) {
                mmp_list_delete_withdata(&ret, config_module_destroy_v);
                goto done;
            }
        }
    }
done:
    return ret;
}

static t_config_server_s *disobj_to_config_server(t_disobj_s *obj)
{
    t_config_server_s *ret;
    t_diselem_s *de;
    t_mmp_listelem_s *p;
    if (obj==NULL || strcmp(obj->name, "server_settings")!=0) {
        mmp_setError(MMP_ERR_PARAMS);
        return NULL;
    }
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    ret->listen_port = ret->listen_queue = 0;
    ret->tmp_dir = ret->pid_file = NULL;
    ret->uid = ret->gid = ret->keepalive_timeout = 0;
    ret->http_root = ret->default_page = ret->log_file = ret->err_file =
        ret->server_meta = ret->modules_basepath = NULL;
    for (p=obj->elemlist->head; p!=NULL; p=p->next) {
        if ((de = (t_diselem_s *)p->data)==NULL)
            continue; /* try to go on anyway */
        if (de->type==OT_KV) {
            if (parse_config_server_kv(ret, de->elem.kv)!=MMP_ERR_OK) {
                config_server_destroy(&ret);
                goto done;
            }
        } else {
            /* we expect no objects here */
            config_server_destroy(&ret);
            goto done;
        }
    }
done:
    return ret;
}

static ret_t parse_config_obj(t_config_s *config, t_disobj_s *obj)
{
    if (config==NULL || obj==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if (!strcmp(obj->name, "server_settings")) {
        t_config_server_s *config_server;
        if ((config_server = disobj_to_config_server(obj))==NULL)
            return MMP_ERR_PARSE;
        config->server = config_server;
        return MMP_ERR_OK;
    } else if (!strcmp(obj->name, "modules")) {
        t_mmp_list_s *config_module;
        if ((config_module = disobj_to_config_module(obj))==NULL)
            return MMP_ERR_PARSE;
        config->modules = config_module;
        return MMP_ERR_OK;
    } else {
        char buf[0xff];
        sprintf(buf, "Unrecognized group %.80s\n", obj->name);
        mmp_setError_ext(MMP_ERR_PARSE, buf);
        return MMP_ERR_PARSE;
    }
}

t_config_s *disobj_to_config(t_disobj_s *obj)
{
    t_config_s *ret;
    t_diselem_s *de;
    t_mmp_listelem_s *p;
    if (obj==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return NULL;
    }
    if (strcmp(obj->name, "config")!=0) {
        mmp_setError(MMP_ERR_PARSE);
        return NULL;
    }
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    ret->server = NULL;
    ret->modules = NULL;
    for (p=obj->elemlist->head; p!=NULL; p=p->next) {
        if ((de = (t_diselem_s *)p->data)==NULL)
            continue; /* try to go on anyway */
        if (de->type==OT_KV) {
            /* we expect no k/v in here */
            config_destroy(&ret);
            goto done;
        } else {
            if (parse_config_obj(ret, de->elem.obj)!=MMP_ERR_OK) {
                config_destroy(&ret);
                goto done;
            }
        }
    }
done:
    return ret;
}

