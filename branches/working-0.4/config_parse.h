#ifndef H_CONFIG_PARSE_H
#define H_CONFIG_PARSE_H

#include <mmp/mmp_list.h>
#include <mmp/mmp_string.h>
#include <mmp/mmp_memory.h>
#include <disml/disml.h>
#include "config_type.h"

/* destroy a config object */
void config_destroy(t_config_s **config);
/* get a config object from a disobj object */
t_config_s *disobj_to_config(struct disobj_s *obj);

#endif /* H_CONFIG_PARSE_H */
