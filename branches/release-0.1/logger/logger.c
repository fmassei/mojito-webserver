#include "logger.h"
#include <dlfcn.h>

struct module_logger_s *logger_modules = NULL;

void logmsg(int prio, char *fmt, ...)
{
    struct module_logger_s *p;
    for (p=logger_modules; p!=NULL; p=p->next)
        if (p->f_logmsg!=NULL) {
            va_list argp;
            va_start(argp, fmt);
            p->f_logmsg(prio, fmt, argp);
            va_end(argp);
        }
}

void logflush()
{
    struct module_logger_s *p;
    for (p=logger_modules; p!=NULL; p=p->next)
        if (p->logflush!=NULL)
            p->logflush();
}

int logger_init()
{
    struct module_logger_s *p;
    int ret = 0;
    for (p=logger_modules; p!=NULL; p=p->next)
        if (p->base.module_init!=NULL)
            ret |= p->base.module_init();
    return ret;
}

int logger_fini()
{
    struct module_logger_s *p;
    int ret;
    for (p=logger_modules; p!=NULL; p=p->next)
        if (p->base.module_fini!=NULL)
            ret = p->base.module_fini();
    return ret;
}

void loghit(char *in_ip, char *method_str, char *uri)
{
    struct module_logger_s *p;
    for (p=logger_modules; p!=NULL; p=p->next)
        if (p->loghit!=NULL)
            p->loghit(in_ip, method_str, uri);
}

/* static library "loader" */
struct module_logger_s *logger_add_static_mod(
                                    struct module_logger_s*(*get_module)(void))
{
    struct module_logger_s *p;
    if ((p = get_module())==NULL)
        return NULL;
    p->next = logger_modules;
    logger_modules = p;
    return p;
}

#ifdef DYNAMIC
/* dynamic library loader */
struct module_logger_s *logger_add_dynamic_mod(char *fname, char **error)
{
    struct module_logger_s*(*get_module)(void);
    void *handle;
    if ((handle = dlopen(fname, RTLD_NOW | RTLD_GLOBAL))==NULL) {
        *error = dlerror();
        return NULL;
    }
    dlerror();
    *(void**)(&get_module) = dlsym(handle, "getmodule");
    if ((*error = dlerror())!=NULL)
        return NULL;
    return logger_add_static_mod(get_module);
}
#endif /* DYNAMIC */

