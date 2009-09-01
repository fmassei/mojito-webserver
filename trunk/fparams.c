/*
    Copyright 2009 Francesco Massei

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

#include "fparams.h"

#define PARSE_BLINE     1
#define PARSE_NAME      2
#define PARSE_VALUE     3

/* trim the string */
static char *str_trim(char *str)
{
    int i=0;
    while (*str==' ' || *str=='"')
        ++str;
    for (i=strlen(str)-1; (str[i]==' ' || str[i]=='"') && (i>=0); --i)
        str[i] = '\0';
    return str;
}

/* convert string to int and return 1 on error */
static int assign_to_int(char *value, int *res)
{
    errno = 0;
    *res = strtol(value, (char**)NULL, 10);
    return errno!=0;
}

/* try assign a parameter to a variable */
static int assign_param(char *name, char *value, fparams_st *params)
{
    if ((strlen(name)<=0) || (strlen(value)<=0))
        return -1;
    name = str_trim(name);
    value = str_trim(value);
    if ((strlen(name)<=0) || (strlen(value)<=0))
        return -1;
    if (!strcmp(name, "logfile")) {
        if ((params->logfile = strdup(value))==NULL) return -1;
    } else if (!strcmp(name, "errfile")) {
        if ((params->errfile = strdup(value))==NULL) return -1;
    } else if (!strcmp(name, "pidfile")) {
        if ((params->pidfile = strdup(value))==NULL) return -1;
    } else if (!strcmp(name, "tmp_dir")) {
        if ((params->tmp_dir = strdup(value))==NULL) return -1;
        if (value[strlen(value)-1]=='/')
            params->tmp_dir[strlen(value)-1] = '\0';
    } else if (!strcmp(name, "http_root")) {
        if ((params->http_root = strdup(value))==NULL) return -1;
        if (value[strlen(value)-1]=='/')
            params->http_root[strlen(value)-1] = '\0';
    } else if (!strcmp(name, "default_page")) {
        if ((params->default_page = strdup(value))==NULL) return -1;
    } else if (!strcmp(name, "uid")) {
        if (assign_to_int(value, &params->uid)!=0) {
            fprintf(stderr, "Uid is not a number");
            return -1;
        }
    } else if (!strcmp(name, "gid")) {
        if (assign_to_int(value, &params->gid)!=0) {
            fprintf(stderr, "Gid is not a number");
            return -1;
        }
    } else if (!strcmp(name, "listen_port")) {
        if (assign_to_int(value, &params->listen_port)!=0) {
            fprintf(stderr, "listen_port is not a number");
            return -1;
        }
    } else if (!strcmp(name, "listen_queue")) {
        if (assign_to_int(value, &params->listen_queue)!=0) {
            fprintf(stderr, "listen_queue is not a number");
            return -1;
        }
    } else if (!strcmp(name, "keepalive_timeout")) {
        if (assign_to_int(value, &params->keepalive_timeout)!=0) {
            fprintf(stderr, "keepalive_timeout is not a number");
            return -1;
        }
    } else if (!strcmp(name, "min_compress_filesize")) {
        errno = 0;
        params->min_compress_filesize = strtol(value, (char**)NULL, 10);
        if (errno!=0) {
            fprintf(stderr, "min_compress_filesize is not a number");
            return -1;
        }
    } else if (!strcmp(name, "server_meta")) {
        if ((params->server_meta = strdup(value))==NULL) return -1;
    } else if (!strcmp(name, "module_basepath")) {
        if ((params->module_basepath = strdup(value))==NULL) return -1;
        if (value[strlen(value)-1]=='/')
            params->module_basepath[strlen(value)-1] = '\0';
    } else {
        fprintf(stderr, "Unrecognized ini option %s\n", name);
        return -1;
    }
    return 0;
}

/* check if we have enough parameters, and set the default values where
 * possible */
static int check_fparams(fparams_st *params)
{
    int err = 0;
    if (params->logfile==NULL) {
        fprintf(stderr, "Missing config option \"logfile\"\n");
        err = -1;
    }
    if (params->pidfile==NULL) {
        fprintf(stderr, "Missing config option \"pidfile\"\n");
        err = -1;
    }
    if (params->tmp_dir==NULL) {
        fprintf(stderr, "Missing config option \"tmp_dir\"\n");
        fprintf(stderr, "\tSetting to default (/tmp)\n");
        params->tmp_dir = strdup("/tmp");
    }
    if (params->http_root==NULL) {
        fprintf(stderr, "Missing config option \"http_root\"\n");
        err = -1;
    }
    if (params->default_page==NULL) {
        fprintf(stderr, "Missing config option \"default_page\"\n");
        err = -1;
    }
    if (params->uid==0) {
        fprintf(stderr, "Missing config option \"uid\"\n");
        err = -1;
    }
    if (params->gid==0) {
        fprintf(stderr, "Missing config option \"gid\"\n");
        err = -1;
    }
    if (params->listen_port==0) {
        fprintf(stderr, "Missing config option \"listen_port\"\n");
        fprintf(stderr, "\tSetting to default (8080)\n");
        params->listen_port = 8080;
    }
    if (params->listen_queue==0) {
        fprintf(stderr, "Missing config option \"listen_queue\"\n");
        fprintf(stderr, "\tSetting to default (100)\n");
        params->listen_queue = 100;
    }
    if (params->keepalive_timeout==0) {
        fprintf(stderr, "Missing config option \"keepalive_timeout\"\n");
        fprintf(stderr, "\tSetting to default (3)\n");
        params->keepalive_timeout = 3;
    }
    if (params->min_compress_filesize==0) {
        fprintf(stderr, "Missing config option \"min_compress_filesize\"\n");
        fprintf(stderr, "\tSetting to default (20000)\n");
        params->min_compress_filesize = 20000;
    }
    if (params->server_meta==NULL) {
        fprintf(stderr, "Missing config option \"server_meta\"\n");
        fprintf(stderr, "\tSetting to default Mojito/0.1\n");
        /* strdup() here for consistency on params_free() */
        params->server_meta = strdup("Mojito/0.1");
    }
    if (params->module_basepath==NULL) {
        fprintf(stderr, "Missing config option \"module_basepath\"\n");
        err = -1;
    }
    return err;
}

/* zero fparams_st struct */
static void zero_fparams(fparams_st *params)
{
    if (params==NULL) return;
    params->logfile = NULL;
    params->errfile = NULL;
    params->pidfile = NULL;
    params->tmp_dir = NULL;
    params->http_root = NULL;
    params->default_page = NULL;
    params->uid = params->gid = 0;
    params->listen_port = params->listen_queue = 0;
    params->keepalive_timeout = 0;
    params->min_compress_filesize = 20000;
    params->server_meta = NULL;
    params->module_basepath = NULL;
    params->mod_params = NULL;
}

static int add_section(fparams_st *params, char *bname)
{
    struct module_params_s *p;
    if ((p = malloc(sizeof(*p)))==NULL)
        return -1;
    if ((p->name = strdup(bname))==NULL) {
        free(p);
        return -1;
    }
    p->params = NULL;
    p->next = params->modparams;
    params->modparams = p;
    return 0;
}

/* quick and dirty INI parser */
int params_loadFromINIFile(const char *fname, fparams_st *params)
{
    struct stat sb;
    int fd, len, i, state;
    char *addr, c;
    char *bname=NULL, *bval=NULL;
    if (params==NULL) return -1;
    zero_fparams(params);
    if (stat(fname, &sb)<0) return -1;
    if ((fd = open(fname, O_RDONLY))<0) return -1;
    len = sb.st_size;
    addr = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    state = PARSE_BLINE;
    for (i=0; i<len; ++i) {
        c = *(addr+i);
        switch(state) {
        case PARSE_BLINE:
            if (c==';') {
                for (;((i<len)&&(*(addr+i)!='\n')); ++i) ;
                continue;
            } else if (c=='\n') {
                continue;
            } else if (c=='[') {
                bname = addr+i;
                for(;((i<len)&&(*(addr+i)!='\n')&&(*(addr+i)!=']')); ++i) ;
                addr[i++]='\0';
                if (add_section(params, bname)<0)
                    goto done;
            }
            bname = addr+i;
            state = PARSE_NAME;
            break;
        case PARSE_NAME:
            if (c=='=') {
                addr[i] = '\0';
                bval = addr+(++i);
                state = PARSE_VALUE;
            }
            break;
        case PARSE_VALUE:
            if (c=='\n') {
                addr[i] = '\0';
                if (params->mod_params==NULL) {
                    if (assign_param(bname, bval, params)<0)
                        goto done;
                } else {
                    if (plist_insert(&(params->mod_params->params),
                                                            bname, bval, 0)<0)
                        goto done;
                }
                state = PARSE_BLINE;
            }
            break;
        }
    }
done:
    munmap(addr, len);
    close(fd);
    if (state!=PARSE_BLINE)
        return -2;
    if (check_fparams(params)!=0)
        return -2;
    return 0;
}

/* free parameters */
void params_free(fparams_st *params)
{
    struct module_params_s *p, *q;
    if (params->logfile) free(params->logfile);
    if (params->errfile) free(params->errfile);
    if (params->pidfile) free(params->pidfile);
    if (params->http_root) free(params->http_root);
    if (params->default_page) free(params->default_page);
    if (params->tmp_dir) free(params->tmp_dir);
    if (params->server_meta) free(params->server_meta);
    if (params->module_basepath) free(params->module_basepath);
    for (p=params->mod_params; p!=NULL; p=q) {
        q = p->next;
        if (p->name) free(p->name);
        if (p->params) plist_destroy(&(p->params));
        free(p);
    }
}

/* return the given parameter module (if any) */
struct module_params_s *params_getModuleParams(fparams_st *params, char *name)
{
    struct module_params_s *p;
    for (p=params->mod_params; p!=NULL; p=p->next)
        if (!strcmp(p->name, name))
            return p;
    return NULL;
}

