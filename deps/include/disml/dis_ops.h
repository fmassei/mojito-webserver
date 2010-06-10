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
#ifndef H_DIS_OPS_H
#define H_DIS_OPS_H

#include <mmp_string.h>
#include "disml.h"
#include "dis_types.h"

void diskv_delete(t_diskv_s **kv);
void disobj_delete(t_disobj_s **obj);
void diselem_delete(t_diselem_s **elem);

t_diselem_s *kv_to_elem(t_diskv_s *kv);
t_diselem_s *obj_to_elem(t_disobj_s *obj);
t_disobj_s *disobj_find_topmost(t_disobj_s *obj);

DISML_API void disobj_destroy(t_disobj_s **obj);
DISML_API void disobj_smart_sort(t_disobj_s *obj);

#endif /* H_DIS_OPS_H */

