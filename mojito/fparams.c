#include "fparams.h"

static void params_zero(struct fparam_s *params)
{
    params->pidfile = params->http_root = params->tmp_dir =
        params->default_page = params->server_meta = params->logfile =
        params->errfile = NULL;
    params->uid = params->gid = 0;
    params->listen_port = params->listen_queue = params->keepalive_timeout = 0;
}

struct void params_free(struct fparam_s **params)
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
    *ret = strtol(value, (char_t**)NULL, 10);
    return errno!=0;
}

static int_t check_fparams(struct fparam_s *params)
{
    int_t err = 0;
    if (params->tmp_dir==NULL) {
        mjt_logger(LOG_ALERT, "INI: missing config option \"tmp_dir\", "
                                "defaulting to (/tmp)\n");
        if (mjt_isrwdir("/tmp")==FALSE) {
            mjt_logger(LOG_CRIT, "INI: /tmp dir not accessible.\n");
            return -1;
        }
        params->tmp_dir = strdup("/tmp");
    }
}

struct fparam_s *params_loadFromINIFile(const char_t *fname)
{
    struct fparams_s *ret;
    struct inisection_s *ini, *q;
    struct kvlist_s *par;
    if ((ini = mjt_iniparse(fname))==NULL)
        return NULL;
    if ((ret = mjt_malloc(sizeof(*ret)))==NULL)
        return NULL;
    params_zero(ret);
    for (q=ini; q!=NULL; q=q->next) {
        if (strncasecmp(q->name, "main", strlen(q->name)))
            continue;
        for (par=q->params; par!=NULL; par=par->next) {
            if (!strncasecmp(par->key, "pidfile", strlen(par->key))) {
                if ((ret->pidfile = strdup(par->val))==NULL)
                    goto baderror;
            } else if (!strncasecmp(par->key, "tmp_dir", strlen(par->key))) {
                if ((ret->tmp_dir = strdup(par->val))==NULL)
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
    mjt_inifree(&ini);
    return ret;
baderror:
    mjt_inifree(&ini);
    params_free(&ret);
    return NULL;
}

