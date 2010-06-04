#include "config_manager.h"

/* Mojito configuration object. The one and only. */
static t_config_s *s_config = NULL;

/* get _the_ configuration object. */
const t_config_s * const config_get(void)
{
    return s_config;
}

/* check if the config values are ok, trying to use the default values for
 * missing ones */
static ret_t check_config(void)
{
    if (s_config==NULL || s_config->server==NULL)
        return MMP_ERR_GENERIC;
    if (s_config->server->listen_port==0) {
        printf("config_manager: missing listen_port, defaulting to %d\n",
            DEFAULT_LISTEN_PORT);
        s_config->server->listen_port = DEFAULT_LISTEN_PORT;
    }
    if (s_config->server->listen_queue==0) {
        printf("config_manager: missing listen_queue, defaulting to %d\n",
            DEFAULT_LISTEN_PORT);
        s_config->server->listen_queue = DEFAULT_LISTEN_QUEUE;
    }
    if (s_config->server->tmp_dir==NULL) {
        printf("config_manager: missing tmp_dir, defaulting to %s\n",
            DEFAULT_TMP_DIR);
        s_config->server->tmp_dir = xstrdup(DEFAULT_TMP_DIR);
    }
    if (s_config->server->keepalive_timeout==0) {
        printf("config_manager: missing keepalive_timeout, defaulting to %d\n",
            DEFAULT_KEEPALIVE_TO);
        s_config->server->keepalive_timeout = DEFAULT_KEEPALIVE_TO;
    }
    if (s_config->server->default_page==NULL) {
        printf("config_manager: missing deafault_page, defaulting to %s\n",
            DEFAULT_DEFAULT_PAGE);
        s_config->server->default_page = xstrdup(DEFAULT_DEFAULT_PAGE);
    }
    /* no warning opts */
    if (s_config->server->server_meta==NULL) {
        s_config->server->server_meta = xstrdup(DEFAULT_SERVER_META);
    }
    if (s_config->server->num_socket_units==0) {
        s_config->server->num_socket_units = DEFAULT_NUM_SU;
    }
    if (s_config->server->num_socket_units_fds==0) {
        s_config->server->num_socket_units_fds = DEFAULT_NUM_SU_FDS;
    }
    return MMP_ERR_OK;
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

