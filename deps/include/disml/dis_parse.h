#ifndef H_DIS_PARSE_H
#define H_DIS_PARSE_H

#include "disml.h"
#include "dis_types.h"
#include "dis_ops.h"

DISML_API struct disobj_s *dis_parse_file(const char *fname);
DISML_API char *dis_get_parse_error(void);

#endif /* H_DIS_PARSE_H */
