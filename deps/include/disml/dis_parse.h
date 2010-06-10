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
#ifndef H_DIS_PARSE_H
#define H_DIS_PARSE_H

#include "disml.h"
#include "dis_types.h"
#include "dis_ops.h"

DISML_API t_disobj_s *dis_parse_file(const char *fname);
DISML_API char *dis_get_parse_error(void);

#endif /* H_DIS_PARSE_H */
