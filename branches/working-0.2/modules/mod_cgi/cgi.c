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

#include "cgi.h"

static char **nuenv = NULL;
static int nenv = 0;

/* add a variable to the new environment */
static int add_env(char *entry)
{
    if ((nuenv = realloc(nuenv, (nenv+1)*sizeof(char*)))==NULL)
        return -1;
    if (entry!=NULL) {
        if ((nuenv[nenv++] = strdup(entry))==NULL)
            return -1;
    } else
        nuenv[nenv++] = (char*)0;
    return 0;
}

/* add a key-value(as string) pair to the environment */
static int add_env_kv(char *key, char *val)
{
    char *p;
    p = malloc(strlen(key)+strlen(val)+2);
    sprintf(p, "%s=%s", key, val);
    if (add_env(p)!=0) {
        free(p);
        return -1;
    }
    free(p);
    return 0;
}

/* add a key-value(as long) to the environment */
static int add_env_kl(char *key, long val)
{
    char *p;
    p = malloc(strlen(key)+15+2);
    sprintf(p, "%s=%lu", key, val);
    if (add_env(p)!=0) {
        free(p);
        return -1;
    }
    free(p);
    return 0;
}

/* prepare a standard cgi environment */
static int prepare_env(struct request_s *req)
{
    if (req->qs!=NULL)
        if (add_env_kv("QUERY_STRING", req->qs)!=0) return -1;
    if (add_env("SERVER_SOFTWARE=mojito/0.1")!=0) return -1;
    if (add_env("GATEWAY_INTERFACE=CGI/1.1")!=0) return -1;
    /* FIXME change this to the request protocol setting */
    if (add_env("SERVER_PROTOCOL=HTTP/1.0")!=0) return -1;
    /* FIXME: somehow these are not working for PHP scripts */
/*    if (add_env("PATH_INFO=/")!=0) return -1;
    if (add_env_kv("PATH_TRANSLATED", dirname(filename))!=0) return -1;
    if (add_env_kv("SCRIPT_NAME", basename(filename))!=0) return -1;*/
    if (add_env_kv("REQUEST_METHOD", req->method_str)!=0) return -1;
    if (add_env_kv("SCRIPT_FILENAME", req->abs_filename)!=0) return -1;
    if (!strcmp(req->method_str, "POST") && req->content_type!=NULL) {
        if (add_env_kv("CONTENT_TYPE", req->content_type)!=0) return -1;
        if (add_env_kl("CONTENT_LENGTH", req->content_length)!=0) return -1;
    }
    if (add_env(NULL)!=0) return -1;
    return 0;
}

/* run a cgi instance */
int cgi_run(struct request_s *req, int sock)
{
    pid_t pid;
    logmsg(LOG_INFO, "cgi run %s", req->abs_filename);
    pid = fork();
    if (pid==-1) {
        return -1;
    } else if (pid==0) {
        if (prepare_env(req)!=0) {
            return -1;
        }
        /* put post_fd on stdin */
        if (req->post_fd!=0) {
            lseek(req->post_fd, 0, SEEK_SET);
            if (dup2(req->post_fd, 0)==-1) {
                perror("dup2");
                return -1;
            }
        }
        /* sock on stdout */
        if (dup2(sock, 1)==-1) {
            perror("dup2");
            return -1;
        }
        header_push_code(HRESP_200);
        header_part_send(sock);
        execle(req->abs_filename, basename(req->abs_filename), (char*)0, nuenv);
        perror("execl");
        return -1;
    } else {
        /* log and die - the child will take care of the rest */
        loghit(req->in_ip, req->method_str, req->uri);
        exit(0);
    }
    return 0; /* it shouldn't get here */
}

