/*
    Copyright 2010 Francesco Massei

    This file is part of mojito webserver.

        Mojito is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Mojito is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Mojito.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef H_CONFIG_TYPE_H
#define H_CONFIG_TYPE_H

#include <mmp/mmp_list.h>

/* server main options */
typedef struct config_server_s {
    int listen_port;
    int listen_queue;
    char *interface;
    char *tmp_dir;
    char *pid_file;
    int uid, gid;
    int keepalive_timeout;
    int keepalive_max;
    char *http_root;
    char *default_page;
    int log_on_file;
    char *log_file;
    char *err_file;
    char *server_meta;
    char *modules_basepath;
    int num_socket_units;
    int num_socket_units_fds;
} t_config_server_s;

/* a single module setting conf. We have a list of these in t_config_module_s */
typedef struct config_module_setting_s {
    char *key;
    char *val;
} t_config_module_setting_s;

/* a single module conf. We have a list of these in t_config_s */
typedef struct config_module_s {
    char *name;
    t_mmp_list_s *settings;
} t_config_module_s;

/* a Mojito configuration object */
typedef struct config_s {
    t_config_server_s   *server;
    t_mmp_list_s       *modules;
} t_config_s;

#endif /* H_CONFIG_TYPE_H */
