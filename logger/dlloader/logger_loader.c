#include "../logger.h"
#include <dlfcn.h>

/* internal function pointers */
static void (*lib_logger_set_global_parameters)(fparams_st *) = NULL;
static void (*_f_lib_logmsg)(int, char *, va_list) = NULL;
static void (*lib_logflush)(void) = NULL;
static void (*lib_loghit)(char *, char *, char *) = NULL;

/* function wrappers */
void logmsg(int prio, char *fmt, ...)
{
    va_list argp;
    if (_f_lib_logmsg==NULL)
        return;
    va_start(argp, fmt);
    _f_lib_logmsg(prio, fmt, argp);
    va_end(argp);
}

void logflush()
{
    if (lib_logflush!=NULL)
        lib_logflush();
}

void loghit(char *in_ip, char *method_str, char *uri)
{
    if (lib_loghit!=NULL)
        lib_loghit(in_ip, method_str, uri);
}

void logger_set_global_parameters(fparams_st *pars)
{
    if (lib_logger_set_global_parameters!=NULL)
        lib_logger_set_global_parameters(pars);
}

/* library loader */
int logger_loadmod(char *fname, char **error)
{
    void *handle;
    if ((handle = dlopen(fname, RTLD_NOW | RTLD_GLOBAL))==NULL) {
        *error = dlerror();
        return -1;
    }
    dlerror();
    *(void**)(&lib_logger_set_global_parameters) = 
                                dlsym(handle, "logger_set_global_parameters");
    if ((*error = dlerror())!=NULL) return -1;
    *(void**)(&_f_lib_logmsg) = dlsym(handle, "_f_logmsg");
    if ((*error = dlerror())!=NULL) return -1;
    *(void**)(&lib_logflush) = dlsym(handle, "logflush");
    if ((*error = dlerror())!=NULL) return -1;
    *(void**)(&lib_loghit) = dlsym(handle, "loghit");
    if ((*error = dlerror())!=NULL) return -1;
    return 0;
}

