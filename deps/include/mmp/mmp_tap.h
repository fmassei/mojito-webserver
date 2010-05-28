#ifndef H_MMP_TAP_H
#define H_MMP_TAP_H

#include <stdio.h>
#include <stdlib.h>

enum mmp_tap_result_e {
    MMP_TAP_PASSED,
    MMP_TAP_FAILED,
    MMP_TAP_UNTESTED
};
struct mmp_tap_entry_s {
    enum mmp_tap_result_e result;
    char *desc;
    char *comment;
};
struct mmp_tap_cycle_s {
    char *name;
    struct list_s *tests;
};

#include "mmp_memory.h"
#include "mmp_string.h"
#include "mmp_list.h"
#include "mmp_trace.h"

struct mmp_tap_cycle_s *mmp_tap_startcycle(const char * const name);
ret_t mmp_tap_test(struct mmp_tap_cycle_s *cycle, const char * const desc,
                        const char * const comment, enum mmp_tap_result_e res);
ret_t mmp_tap_test_passed(struct mmp_tap_cycle_s *cycle,
                        const char * const desc, const char * const comment);
ret_t mmp_tap_test_failed(struct mmp_tap_cycle_s *cycle,
                        const char * const desc, const char * const comment);
ret_t mmp_tap_test_skipped(struct mmp_tap_cycle_s *cycle,
                        const char * const desc, const char * const comment);
void mmp_tap_freecycle(struct mmp_tap_cycle_s **cycle);

ret_t mmp_tap_print(struct mmp_tap_cycle_s *cycle, FILE *out);

#endif /* H_MMP_TAP_H */
