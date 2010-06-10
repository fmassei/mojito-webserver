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
#ifndef H_DIS_TYPES_H
#define H_DIS_TYPES_H

#include <mmp_list.h>

typedef struct diskv_s {
    char *key, *val;
} t_diskv_s;

typedef struct diselem_s {
    enum disobtype_e { OT_KV, OT_OBJ } type;
    union diselem_u {
        struct diskv_s *kv;
        struct disobj_s *obj;
    } elem;
} t_diselem_s;

typedef struct disobj_s {
    char *name;
    t_mmp_list_s *elemlist;
    struct disobj_s *parent;
} t_disobj_s;

#endif /* H_DIS_TYPES_H */

