#ifndef H_DIS_TYPES_H
#define H_DIS_TYPES_H

#include <mmp/mmp_list.h>

struct diskv_s {
    char *key, *val;
};
struct diselem_s {
    enum disobtype_e { OT_KV, OT_OBJ } type;
    union diselem_u {
        struct diskv_s *kv;
        struct disobj_s *obj;
    } elem;
};
struct disobj_s {
    char *name;
    struct list_s *elemlist;
    struct disobj_s *parent;
};

#endif /* H_DIS_TYPES_H */

