#ifndef H_DISML_H
#define H_DISML_H

#include <mmp_h_utils.h>

#ifndef _WIN32
#   define DISML_API    extern
#else
#   ifdef DISML_EXPORTS
#       define DISML_API __declspec(dllexport)
#   else
#       define DISML_API __declspec(dllimport)
#   endif
#endif

#include "dis_types.h"
#include "dis_ops.h"
#include "dis_parse.h"

#define DISML_MAGIC "DISML"

MMP_CEXTERN_BEGIN

DISML_API struct disobj_s *dis_parse_file(const char *fname);
DISML_API char *dis_get_parse_error(void);

DISML_API ret_t dis_print_file(char *fname, struct disobj_s *obj);

DISML_API void disobj_smart_sort(struct disobj_s *obj);

DISML_API void disobj_destroy(struct disobj_s **obj);

DISML_API void dis_reset_error_tracing(void);
DISML_API void dis_print_error_tracing(void);

MMP_CEXTERN_END

#endif /* H_DISML_H */
