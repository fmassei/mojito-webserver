#ifndef H_MMP_DIR_H
#define H_MMP_DIR_H

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#   include <sys/types.h>
#   include <dirent.h>
#else
#   include <windows.h>
#endif

#include "mmp_list.h"
#include "mmp_trace.h"
#include "mmp_memory.h"
#include "mmp_string.h"

/* list all files in a directory */
struct list_s *mmp_dir_listfiles(const char * const dirname);

/* free the results of a mmp_dir_listfiles */
void mmp_dir_free_list_results(struct list_s **list);

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_dir_unittest(struct mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_DIR_H */
