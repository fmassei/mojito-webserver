#include "../cache.h"
#include <dlfcn.h>

/* internal function pointers */
static struct cache_entry_s *(*lib_cache_lookup)(const char*, const char*)
                                                                        = NULL;
static int (*lib_cache_create_file)(const char*, char*, char*) = NULL;
static void (*lib_cache_set_global_parameters)(fparams_st *) = NULL;
static int (*lib_cache_init)(void) = NULL;
static void (*lib_cache_fini)(void) = NULL;

/* function wrappers */
struct cache_entry_s *cache_lookup(const char *URI, const char *filter_id)
{
    if (lib_cache_lookup!=NULL)
        return lib_cache_lookup(URI, filter_id);
    return NULL;
}

int cache_create_file(const char *URI, char *filter_id, char *content_type)
{
    if (lib_cache_create_file!=NULL)
        return lib_cache_create_file(URI, filter_id, content_type);
    return -1;
}

int cache_init()
{
    if (lib_cache_init!=NULL)
        return lib_cache_init();
    return -1;
}

void cache_fini()
{
    if (lib_cache_fini!=NULL)
        lib_cache_fini();
}

/* set global parameters */
void cache_set_global_parameters(fparams_st *params)
{
    if (lib_cache_set_global_parameters!=NULL)
        lib_cache_set_global_parameters(params);
}

/* library loader */
int cache_loadmod(char *fname, char **error)
{
    void *handle;
    if ((handle = dlopen(fname, RTLD_NOW | RTLD_GLOBAL))==NULL) {
        *error = dlerror();
        return -1;
    }
    dlerror();
    *(void**)(&lib_cache_init) = dlsym(handle, "cache_init");
    if ((*error = dlerror())!=NULL) return -1;
    *(void**)(&lib_cache_fini) = dlsym(handle, "cache_fini");
    if ((*error = dlerror())!=NULL) return -1;
    *(void**)(&lib_cache_set_global_parameters) =
                                dlsym(handle, "cache_set_global_parameters");
    if ((*error = dlerror())!=NULL) return -1;
    *(void**)(&lib_cache_lookup) = dlsym(handle, "cache_lookup");
    if ((*error = dlerror())!=NULL) return -1;
    *(void**)(&lib_cache_create_file) = dlsym(handle, "cache_create_file");
    if ((*error = dlerror())!=NULL) return -1;
    return 0;
}

