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

static struct module_s *modules = NULL;
struct module_filter_s *filters = NULL;

/* delete a module from the queue.
 * NOTE1: the modules will NOT be unloaded!
 * NOTE2: don't change pointer of *todrop, as it will be impossible to manage
 *      module-drops in a cycle! */
static void dropmod(struct module_s **head, struct module_s *todrop)
{
    if (todrop==*head) {
        *head = todrop->next;
        if (*head!=NULL)
            (*head)->prev = todrop->prev;
    } else {
        if (todrop->prev!=NULL)
            todrop->prev->next = todrop->next;
        if (todrop->next!=NULL)
            todrop->next->prev = todrop->prev;
    }
}

/* FIXME horrible style, but I didn't find a better way... */
/* TODO FIXME XXX try not to unload modules just free()ing them! */
#define MOD_LOOP_HEAD \
    struct module_s *p, *q; \
    int ret; \
    p = modules; \
    while (p!=NULL) {
#define MOD_LOOP_SKIP_STOPPED \
        if (p->will_run==0) \
            continue;
#define MOD_LOOP_SWITCH \
        switch(ret) { \
        case MOD_NOHOOK: \
            break; \
        case MOD_CORE_CRIT: \
            return -1;
#define MOD_LOOP_CASE_CRIT \
        case MOD_CRIT: \
            dropmod(&modules, p); \
            if (p->prev==NULL) { \
                free(p); \
                p = modules; \
                continue; \
            } else { \
                q = p->next; \
                free(p); \
                p = q; \
                continue; \
            }
#define MOD_LOOP_CASE_ERR \
        case MOD_ERR: \
            p->will_run = 0; \
            break;
#define MOD_LOOP_CASE_PROCDONE \
        case MOD_PROCDONE: \
            return 0;
#define MOD_LOOP_CASE_ALLDONE \
        case MOD_ALLDONE: \
            return 1;
#define MOD_LOOP_END \
        default: \
            break; \
        } \
        p = p->next; \
    }

#define MOD_LOOP_NORMFLOW \
    MOD_LOOP_SWITCH \
    MOD_LOOP_SKIP_STOPPED \
    MOD_LOOP_CASE_CRIT \
    MOD_LOOP_CASE_ERR \
    MOD_LOOP_CASE_PROCDONE \
    MOD_LOOP_CASE_ALLDONE \
    MOD_LOOP_END
 
int mod_set_params(struct plist_s *params)
{
    MOD_LOOP_HEAD
        ret = (p->set_params!=NULL) ? p->set_params(params) : MOD_NOHOOK;
    MOD_LOOP_SWITCH
    MOD_LOOP_CASE_CRIT
    MOD_LOOP_CASE_ERR
    MOD_LOOP_END
    return 0;
}

int mod_init(void)
{
    MOD_LOOP_HEAD
        ret = (p->init!=NULL) ? p->init() : MOD_NOHOOK;
    MOD_LOOP_SWITCH
    MOD_LOOP_CASE_CRIT
    MOD_LOOP_CASE_ERR
    MOD_LOOP_END
    return 0;
}

int mod_fini(void)
{
    MOD_LOOP_HEAD
        ret = (p->fini!=NULL) ? p->fini() : MOD_NOHOOK;
    MOD_LOOP_SWITCH
    MOD_LOOP_CASE_CRIT
    MOD_LOOP_CASE_ERR
    MOD_LOOP_END
    return 0;
}

int can_run(struct request_s *req)
{
    MOD_LOOP_HEAD
        ret = (p->can_run!=NULL) ? p->can_run(req) : MOD_NOHOOK;
    MOD_LOOP_NORMFLOW
    return 0;
}

int on_accept(void)
{
    MOD_LOOP_HEAD
        ret = (p->on_accept!=NULL) ? p->on_accept() : MOD_NOHOOK;
    MOD_LOOP_NORMFLOW
    return 0;
}

int on_presend(int sock, struct request_s *req)
{
    MOD_LOOP_HEAD
        ret = (p->on_presend!=NULL) ? p->on_presend(sock, req) : MOD_NOHOOK;
    MOD_LOOP_NORMFLOW
    return 0;
}

int on_prehead(struct stat *sb)
{
    extern struct module_s *ch_filter;
    MOD_LOOP_HEAD
        if ((p->category==MODCAT_FILTER) && (p!=ch_filter)) {
            ret = MOD_NOHOOK;
        } else {
            ret = (p->on_prehead!=NULL) ? p->on_prehead(sb) : MOD_NOHOOK;
        }
    MOD_LOOP_NORMFLOW
    return 0;
}

int on_send(void *addr, int sock, struct stat *sb)
{
    extern struct module_s *ch_filter;
    MOD_LOOP_HEAD
        if ((p->category==MODCAT_FILTER) && (p!=ch_filter)) {
            ret = MOD_NOHOOK;
        } else {
            ret = (p->on_send!=NULL) ? p->on_send(addr, sock, sb) : MOD_NOHOOK;
        }
    MOD_LOOP_NORMFLOW
    return 0;
}

int on_postsend(struct request_s *req, char *mime, void *addr, struct stat *sb)
{
    MOD_LOOP_HEAD
        ret = (p->on_postsend!=NULL) ? p->on_postsend(req, mime, addr, sb) : MOD_NOHOOK;
    MOD_LOOP_NORMFLOW
    return 0;
}

struct module_s *module_add_static(struct module_s *(*get_module)(void))
{
    struct module_s *p;
    if ((p = get_module())==NULL)
        return NULL;
    p->next = modules;
    if (modules!=NULL)
        modules->prev = p;
    p->prev = NULL;
    modules = p;
    if (p->category==MODCAT_FILTER) {
        struct module_filter_s *q;
        if ((q = malloc(sizeof(*q)))==NULL) {
            /* TODO check me */
        }
        q->mod = p;
        q->next = filters;
        filters = q;
    }
    return p;
}

#ifdef DYNAMIC
struct module_s *module_add_dynamic(char *fname, char **error)
{
    struct module_s*(*get_module)(void);
    void *handle;
    if ((handle = dlopen(fname, RTLD_NOW | RTLD_GLOBAL))==NULL) {
        *error = dlerror();
        return NULL;
    }
    dlerror();
    *(void**)(&get_module) = dlsym(handle, "getmodule");
    if ((*error = dlerror())!=NULL)
        return NULL;
    return module_add_static(get_module);
}
#endif /* DYNAMIC */

