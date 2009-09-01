#include "cache.h"
#include <dlfcn.h>

struct module_cache_s *cache_modules = NULL;

/* function wrappers */
struct cache_entry_s *cache_lookup(const char *URI, const char *filter_id)
{
    struct module_cache_s *p;
    struct cache_entry_s *q = NULL;
    for (p=cache_modules; p!=NULL; p=p->next)
        if (p->cache_lookup!=NULL)
            q = p->cache_lookup(URI, filter_id);
    return q;
}

int cache_create_file(const char *URI, char *filter_id, char *content_type)
{
    struct module_cache_s *p;
    int ret = -1;
    for (p=cache_modules; p!=NULL; p=p->next)
        if (p->cache_create_file!=NULL)
            ret = p->cache_create_file(URI, filter_id, content_type);
    return ret;
}

int cache_init()
{
    struct module_cache_s *p;
    int ret = -1;
    for (p=cache_modules; p!=NULL; p=p->next)
        if (p->base.module_init!=NULL)
            ret = p->base.module_init();
    return ret;
}

void cache_fini()
{
    struct module_cache_s *p;
    for (p=cache_modules; p!=NULL; p=p->next)
        if (p->base.module_fini!=NULL)
            p->base.module_fini();
}

/* static library "loader" */
struct module_cache_s *cache_add_static_mod(
                                    struct module_cache_s*(*get_module)(void))
{
    struct module_cache_s *p;
    if ((p = get_module())==NULL)
        return NULL;
    p->next = cache_modules;
    cache_modules = p;
    return p;
}

#ifdef DYNAMIC
/* dynamic library loader */
struct module_cache_s *cache_add_dynamic_mod(char *fname, char **error)
{
    struct module_cache_s*(*get_module)(void);
    void *handle;
    if ((handle = dlopen(fname, RTLD_NOW | RTLD_GLOBAL))==NULL) {
        *error = dlerror();
        return NULL;
    }
    dlerror();
    *(void**)(&get_module) = dlsym(handle, "getmodule");
    if ((*error = dlerror())!=NULL)
        return NULL;
    return cache_add_static_mod(get_module);
}
#endif /* DYNAMIC */

