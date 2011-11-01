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
#ifndef H_DEFAULTS_H
#define H_DEFAULTS_H

/* default configuration file */
#define DEFAULT_CONFIGFILE  "../../config.disml"

/* config defaults */
#define DEFAULT_LISTEN_PORT     8080
#define DEFAULT_LISTEN_QUEUE    100
#define DEFAULT_TMP_DIR         "/tmp/"
#define DEFAULT_KEEPALIVE_TO    5
#define DEFAULT_KEEPALIVE_MAX   10
#define DEFAULT_DEFAULT_PAGE    "index.html"
#define DEFAULT_SERVER_META     "Mojito 0.4b"
#define DEFAULT_NUM_SU          64
#define DEFAULT_NUM_SU_FDS      64
#define DEFAULT_LPTASK_SEC      4

#endif /* H_DEFAULTS_H */
