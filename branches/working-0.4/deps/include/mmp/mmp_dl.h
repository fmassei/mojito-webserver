#ifndef H_MMP_DL_H
#define H_MMP_DL_H

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#   include <dlfcn.h>
#else
#   include <windows.h>
#endif

#include "mmp_trace.h"

/* open a dinamically loadable library */
void *mmp_dl_open_and_get_fnc(char *filename, char *getfnc_name);

#endif /* H_MMP_DL_H */
