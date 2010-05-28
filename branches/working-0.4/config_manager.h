#ifndef H_CONFIG_MANAGER_H
#define H_CONFIG_MANAGER_H

#include "config_type.h"
#include "config_parse.h"
#include "defaults.h"

/* get _the_ configuration object. */
const t_config_s * const config_get(void);

/* load the configuration from file */
ret_t config_manager_loadfile(const char *filename);
/* free any existing configuration */
void config_manager_freeall(void);

#endif /* H_CONFIG_MANAGER_H */
