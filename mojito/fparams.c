#include "fparams.h"

static const char_t *default_tmp_dir = "/tmp";
static const char_t *default_pidfile = "mojito.pid";
static const char_t *default_http_root = "/var/www";
static const char_t *default_default_page = "index.html";
static const int_t default_uid = 1000;
static const int_t default_gid = 1000;
static const int_t default_listen_port = 8080;
static const int_t default_listen_queue = 100;
static const int_t default_keepalive_timeout = 3;
static const char_t *default_server_meta = PACKAGE_STRING;
static const char_t *default_logfile = "mojito.log";
static const char_t *default_errfile = "mojito.error.log";

static void params_zero(struct fparam_s *params)
{
    params->pidfile = params->http_root = params->tmp_dir =
        params->default_page = params->server_meta = params->logfile =
        params->errfile = NULL;
    params->uid = params->gid = -1;
    params->listen_port = params->listen_queue = 0;
    params->keepalive_timeout = -1;
}

void params_free(struct fparam_s **params)
{
    if (params==NULL || *params==NULL)
        return;
    mjt_free(&((*params)->pidfile));
    mjt_free(&((*params)->http_root));
    mjt_free(&((*params)->default_page));
    mjt_free(&((*params)->tmp_dir));
    mjt_free(&((*params)->server_meta));
    mjt_free(&((*params)->logfile));
    mjt_free(&((*params)->errfile));
    mjt_free2null(params);
}

static int_t assign_to_int(char_t *value, int_t *res)
{
    errno = 0;
    *res = strtol(value, (char_t**)NULL, 10);
    return errno!=0;
}

static void log_ini_err_message(const char_t *optname, const char_t *defval)
{
    mjt_logmsg(LOG_ALERT, "INI: missing config option: \"%s\", "
                            "defaulting to \"%s\"", optname, defval);
}

static void log_ini_err_message_i(const char_t *optname, const int_t defval)
{
    mjt_logmsg(LOG_ALERT, "INI: missing config option: \"%s\", "
                            "defaulting to \"%d\"", optname, defval);
}

static int_t try_to_strdup(char_t **dest, const char_t *src)
{
    if (dest==NULL || src==NULL)
        return -1;
    if ((*dest = strdup(src))==NULL) {
        mjt_logmsg(LOG_CRIT, "INI: system error saving a parameter (%s)",
                                                            strerror(errno));
        return -1;
    }
    return 0;
}

static int_t get_file_with_path(char_t **fname, const char_t *path)
{
    char_t *fpath;
    bool_t has_slash = FALSE;
    /* if the path is absolute return it */
    if ((*fname)[0] == '/')
        return 0;
    /* otherwise append the path */
    if (path[strlen(path)-1]=='/') has_slash = TRUE;
    if ((fpath = mjt_malloc(strlen(*fname)+strlen(path)+
                                                ((has_slash==TRUE)?0:1)))==NULL)
        return -1;
    if (has_slash==TRUE)
        sprintf(fpath, "%s%s", path, *fname);
    else
        sprintf(fpath, "%s/%s", path, *fname);
    mjt_free2null(fname);
    *fname = fpath;
    return 0;
}

static int_t check_fparams(struct fparam_s *params)
{
    int_t err = 0;
    if (params->tmp_dir==NULL) {
        log_ini_err_message("tmp_dir", default_tmp_dir);
        if (try_to_strdup(&params->tmp_dir, default_tmp_dir)!=0)
            ++err;
    }
    if (mjt_isdir(default_tmp_dir)==FALSE || mjt_isrwx(default_tmp_dir)==FALSE){
        mjt_logmsg(LOG_CRIT, "INI: '%s' dir not accessible.\n",
                                                            default_tmp_dir);
        ++err;
    }
    if (params->pidfile==NULL) {
        log_ini_err_message("pidfile", default_pidfile);
        if (try_to_strdup(&params->pidfile, default_pidfile)!=0) {
            ++err;
        } else if (get_file_with_path(&params->pidfile, params->tmp_dir)!=0) {
            mjt_logmsg(LOG_CRIT, "INI: could not resolve file name for option "
                                "%s\n", "pidfile");
            ++err;
        }
    }
    if (params->http_root==NULL) {
        log_ini_err_message("http_root", default_http_root);
        if (try_to_strdup(&params->http_root, default_http_root)!=0) {
            ++err;
        } else if (mjt_isdir(default_http_root)==FALSE) {
            mjt_logmsg(LOG_CRIT, "INI: '%s' is not a directory!",
                                                            default_http_root);
            ++err;
        }
    }
    if (params->default_page==NULL) {
        log_ini_err_message("default_page", default_default_page);
        if (try_to_strdup(&params->default_page, default_default_page)!=0) {
            ++err;
        } else if (strchr(params->default_page, '/')!=NULL) {
            mjt_logmsg(LOG_CRIT, "INI: invalid characters in default_page");
            ++err;
        }
    }
    if (params->server_meta==NULL) {
        log_ini_err_message("server_meta", default_server_meta);
        if (try_to_strdup(&params->server_meta, default_server_meta)!=0) {
            ++err;
        } else if (params->logfile==NULL) {
            log_ini_err_message("logfile", default_logfile);
            if (try_to_strdup(&params->logfile, default_logfile)!=0)
                ++err;
        }
    }
    if (get_file_with_path(&params->logfile, params->tmp_dir)!=0) {
        mjt_logmsg(LOG_CRIT, "INI: could not resolve file name for option %s\n",
                                                                    "logfile");
        ++err;
    }
    if (params->errfile==NULL) {
        log_ini_err_message("errfile", default_errfile);
        if (try_to_strdup(&params->errfile, default_errfile)!=0) {
            ++err;
        } else if (get_file_with_path(&params->errfile, params->tmp_dir)!=0) {
            mjt_logmsg(LOG_CRIT, "INI: could not resolve file name for option "
                                "%s\n", "errfile");
            ++err;
        }
    }
    if (params->uid<0) {
        log_ini_err_message_i("uid", default_uid);
        params->uid = default_uid;
    }
    if (params->gid<0) {
        log_ini_err_message_i("gid", default_gid);
        params->gid = default_gid;
    }
    if (params->listen_port<=0) {
        log_ini_err_message_i("listen_port", default_listen_port);
        params->listen_port = default_listen_port;
    }
    if (params->listen_queue<=0) {
        params->listen_queue = default_listen_queue;
    }
    if (params->keepalive_timeout<0) {
        log_ini_err_message_i("keepalive_timeout", default_keepalive_timeout);
        params->keepalive_timeout = default_keepalive_timeout;
    }
    return err;
}

struct fparam_s *params_loadFromINIFile(const char_t *fname)
{
    struct fparam_s *ret;
    struct inisection_s *ini, *q;
    struct kvlist_s *par;
    if ((ini = mjt_iniparse(fname))==NULL) {
        mjt_logmsg(LOG_ALERT, "INI: error parsing inifile: %s",
                                                            mjt_inigeterror());
        return NULL;
    }
    if ((ret = mjt_malloc(sizeof(*ret)))==NULL)
        return NULL;
    params_zero(ret);
    for (q=ini; q!=NULL; q=q->next) {
        if (strncasecmp(q->name, "main", strlen(q->name)))
            continue;
        for (par=q->params; par!=NULL; par=par->next) {
            if (!strncasecmp(par->key, "tmp_dir", strlen(par->key))) {
                if ((ret->tmp_dir = strdup(par->val))==NULL)
                    goto baderror;
            } else if (!strncasecmp(par->key, "pidfile", strlen(par->key))) {
                if ((ret->pidfile = strdup(par->val))==NULL)
                    goto baderror;
            } else if (!strncasecmp(par->key, "http_root", strlen(par->key))) {
                if ((ret->http_root = strdup(par->val))==NULL)
                    goto baderror;
            } else if (!strncasecmp(par->key, "default_page",
                                                            strlen(par->key))) {
                if ((ret->default_page = strdup(par->val))==NULL)
                    goto baderror;
            } else if (!strncasecmp(par->key, "server_meta",
                                                            strlen(par->key))) {
                if ((ret->server_meta = strdup(par->val))==NULL)
                    goto baderror;
            } else if (!strncasecmp(par->key, "logfile", strlen(par->key))) {
                if ((ret->logfile = strdup(par->val))==NULL)
                    goto baderror;
            } else if (!strncasecmp(par->key, "errfile", strlen(par->key))) {
                if ((ret->errfile = strdup(par->val))==NULL)
                    goto baderror;
            } else if (!strncasecmp(par->key, "uid", strlen(par->key))) {
                if (assign_to_int(par->val, &ret->uid)!=0)
                    goto baderror;
            } else if (!strncasecmp(par->key, "gid", strlen(par->key))) {
                if (assign_to_int(par->val, &ret->gid)!=0)
                    goto baderror;
            } else if (!strncasecmp(par->key, "listen_port",
                                                            strlen(par->key))) {
                if (assign_to_int(par->val, &ret->listen_port))
                    goto baderror;
            } else if (!strncasecmp(par->key, "listen_queue",
                                                            strlen(par->key))) {
                if (assign_to_int(par->val, &ret->listen_queue))
                    goto baderror;
            } else if (!strncasecmp(par->key, "keepalive_timeout",
                                                            strlen(par->key))) {
                if (assign_to_int(par->val, &ret->keepalive_timeout))
                    goto baderror;
            } else {
                mjt_logmsg(LOG_ALERT, "INI: unrecognized option %s\n",par->key);
            }
        }
    }
    if (check_fparams(ret)!=0)
        goto baderror;
    mjt_inifree(&ini);
    return ret;
baderror:
    mjt_inifree(&ini);
    params_free(&ret);
    return NULL;
}

