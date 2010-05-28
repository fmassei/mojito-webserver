#ifndef H_MMP_HTAB_H
#define H_MMP_HTAB_H

#include <stdio.h>
#include <stdlib.h>
#include "mmp_memory.h"
#include "mmp_trace.h"
#include "mmp_string.h"

struct htab_s {
    size_t size;
    struct htab_elem_s **ptrs;
};

struct htab_s *mmp_htab_create(size_t size);
void mmp_htab_destroy(struct htab_s **ptr);
void mmp_htab_destroy_with_data(struct htab_s **ptr, void(*datadel)(void**));
void *mmp_htab_lookup(struct htab_s *htab, char *key);
ret_t mmp_htab_install(struct htab_s *htab, char *key, void *val);
ret_t mmp_htab_delete(struct htab_s *htab, char *key);

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_htab_unittest(struct mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_HTAB_H */
