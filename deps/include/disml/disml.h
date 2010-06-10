/*
    Copyright 2010 Francesco Massei

    This file is part of the DISML parser library.

        DISML parser is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DISML parser is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with DISML parser.  If not, see <http://www.gnu.org/licenses/>.
*/
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

DISML_API t_disobj_s *dis_parse_file(const char *fname);
DISML_API char *dis_get_parse_error(void);

DISML_API ret_t dis_print_file(char *fname, t_disobj_s *obj);

DISML_API void disobj_smart_sort(t_disobj_s *obj);

DISML_API void disobj_destroy(t_disobj_s **obj);

DISML_API void dis_reset_error_tracing(void);
DISML_API void dis_print_error_tracing(void);

MMP_CEXTERN_END

#endif /* H_DISML_H */
