#ifndef H_CONFIG_TYPE_H
#define H_CONFIG_TYPE_H

#include <mmp/mmp_list.h>

/* server main options */
typedef struct config_server_s {
    int listen_port;
    int listen_queue;
    char *tmp_dir;
    char *pid_file;
    int uid, gid;
    int keepalive_timeout;
    char *http_root;
    char *default_page;
    char *log_file;
    char *err_file;
    char *server_meta;
    char *modules_basepath;
} t_config_server_s;

/* a single module setting conf. We have a list of these in t_config_module_s */
typedef struct config_module_setting_s {
    char *key;
    char *val;
} t_config_module_setting_s;

/* a single module conf. We have a list of these in t_config_s */
typedef struct config_module_s {
    char *name;
    struct list_s *settings;
} t_config_module_s;

/* a Mojito configuration object */
typedef struct config_s {
    t_config_server_s   *server;
    struct list_s       *modules;
} t_config_s;

#endif /* H_CONFIG_TYPE_H */
