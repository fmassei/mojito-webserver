#include "config_manager.h"

/* Mojito configuration object. The one and only. */
static t_config_s *s_config = NULL;

/* get _the_ configuration object. */
const t_config_s * const config_get(void)
{
    return s_config;
}

/* load the configuration from file */
ret_t config_manager_loadfile(const char *filename)
{
    struct disobj_s *obj;
    if (filename==NULL || *filename=='\0') {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    dis_reset_error_tracing();
    if ((obj = dis_parse_file(filename))==NULL) {
        dis_print_error_tracing();
        return MMP_ERR_PARSE;
    }
    if ((s_config = disobj_to_config(obj))==NULL) {
        mmp_trace_print(stdout);
        return MMP_ERR_PARSE;
    }
    disobj_destroy(&obj);
    return MMP_ERR_OK;
}

/* free any existing configuration */
void config_manager_freeall(void)
{
    if (s_config!=NULL)
        config_destroy(&s_config);
}

