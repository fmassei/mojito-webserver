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

/* prepare a standard cgi environment */
static int prepare_env(struct request_s *req)
{
    if (req->qs!=NULL)
        if (add_env_kv(nuenv, nenv, "QUERY_STRING", req->qs)!=0) return -1;
    if (add_env(nuenv, nenv, "SERVER_SOFTWARE=mojito/0.1")!=0) return -1;
    if (add_env(nuenv, nenv, "GATEWAY_INTERFACE=CGI/1.1")!=0) return -1;
    /* FIXME change this to the request protocol setting */
    if (add_env(nuenv, nenv, "SERVER_PROTOCOL=HTTP/1.0")!=0) return -1;
    /* FIXME: somehow these are not working for PHP scripts */
/*    if (add_env(nuenv, nenv, "PATH_INFO=/")!=0) return -1;
    if (add_env_kv(nuenv, nenv, "PATH_TRANSLATED", dirname(filename))!=0)
        return -1;
    if (add_env_kv(nuenv, nenv, "SCRIPT_NAME", basename(filename))!=0)
        return -1;*/
    if (add_env_kv(nuenv, nenv, "REQUEST_METHOD", req->method_str)!=0)
        return -1;
    if (add_env_kv(nuenv, nenv, "SCRIPT_FILENAME", req->abs_filename)!=0)
        return -1;
    if (!strcmp(req->method_str, "POST") && req->content_type!=NULL) {
        if (add_env_kv(nuenv, nenv, "CONTENT_TYPE", req->content_type)!=0)
            return -1;
        if (add_env_kl(nuenv, nenv, "CONTENT_LENGTH", req->content_length)!=0)
            return -1;
    }
    if (add_env(nuenv, nenv, NULL)!=0) return -1;
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

