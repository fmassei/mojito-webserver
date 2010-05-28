#ifndef H_DIS_OPS_H
#define H_DIS_OPS_H

#include <mmp/mmp_string.h>
#include "disml.h"
#include "dis_types.h"

void diskv_delete(struct diskv_s **kv);
void disobj_delete(struct disobj_s **obj);
void diselem_delete(struct diselem_s **elem);

struct diselem_s *kv_to_elem(struct diskv_s *kv);
struct diselem_s *obj_to_elem(struct disobj_s *obj);
struct disobj_s *disobj_find_topmost(struct disobj_s *obj);

DISML_API void disobj_smart_sort(struct disobj_s *obj);

#endif /* H_DIS_OPS_H */

