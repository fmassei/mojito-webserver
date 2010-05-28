#ifndef H_MMP_STRING_H
#define H_MMP_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmp_error.h"
#include "mmp_memory.h"

/* unix strdup() */
char *xstrdup(const char *s);
/* unix index() */
char *xindex(char *s, char c);

/* trim functions */
int mmp_str_is_trimmable(char c);   /* is a character "trimmable"? */
char *mmp_str_ltrim(char *str);     /* left trim */
char *mmp_str_rtrim(char *str);     /* right trim */
char *mmp_str_trim(char *str);      /* left and right trim */

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_string_unittest(struct mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_STRING_H */
