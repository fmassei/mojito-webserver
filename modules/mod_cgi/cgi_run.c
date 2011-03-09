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

#include "cgi_run.h"
#include "../../common_src/resp_headers.h"
#include "../../common_src/env_setter.h"
#include "../../src/utils.h"

static char **nuenv = NULL;
static int nenv = 0;

/* prepare a standard cgi environment */
static ret_t prepare_env(t_request_s *req)
{
    if (req->query_string!=NULL)
        if (env_add_kv(nuenv, nenv, "QUERY_STRING", req->query_string)!=MMP_ERR_OK)
            return MMP_ERR_GENERIC;
    if (env_add(nuenv, nenv, "SERVER_SOFTWARE=mojito/0.1")!=0) return -1;
    if (env_add(nuenv, nenv, "GATEWAY_INTERFACE=CGI/1.1")!=0) return -1;
    /* FIXME change this to the request protocol setting */
    if (env_add(nuenv, nenv, "SERVER_PROTOCOL=HTTP/1.0")!=0) return -1;
    /* FIXME: somehow these are not working for PHP scripts */
/*    if (env_add(nuenv, nenv, "PATH_INFO=/")!=0) return -1;
    if (env_add_kv(nuenv, nenv, "PATH_TRANSLATED", dirname(filename))!=0)
        return -1;
    if (env_add_kv(nuenv, nenv, "SCRIPT_NAME", basename(filename))!=0)
        return -1;*/
    if (env_add_kv(nuenv, nenv, "REQUEST_METHOD", request_method_string(req->method))!=0)
        return -1;
    if (env_add_kv(nuenv, nenv, "SCRIPT_FILENAME", req->abs_filename)!=0)
        return -1;
    if (req->method==REQUEST_METHOD_POST && req->content_type!=NULL) {
        if (env_add_kv(nuenv, nenv, "CONTENT_TYPE", req->content_type)!=0)
            return -1;
        if (env_add_kl(nuenv, nenv, "CONTENT_LENGTH", req->content_length)!=0)
            return -1;
    }
    if (env_add(nuenv, nenv, NULL)!=0) return -1;
    return MMP_ERR_OK;
}

/* run a cgi instance */
int cgi_run(t_request_s *req, t_response_s *res)
{
    pid_t pid;
    DBG_PRINT(("[mod_cgi] cgi run %s\n", req->abs_filename));
    pid = fork();
    if (pid==-1) {
        mmp_setError_ext(MMP_ERR_GENERIC, "could not fork!");
        return -1;
    } else if (pid==0) {
        if (prepare_env(req)!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return -1;
        }
        /* put post_fd on stdin */
        if (req->post_fd>=0) {
            lseek(req->post_fd, 0, SEEK_SET);
            if (dup2(req->post_fd, 0)==-1) {
                mmp_setError(MMP_ERR_FILE);
                return -1;
            }
        }
        /* sock on stdout */
        if (dup2(res->sock, 1)==-1) {
            mmp_setError(MMP_ERR_FILE);
            return -1;
        }
        res->sock = 1;
        header_push_code(res, HRESP_200, req->protocol);
        mmp_socket_write(&res->sock, res->resbuf, strlen(res->resbuf));
        /* FIXME: check for basename */
        execle(req->abs_filename, basename(req->abs_filename), (char*)0, nuenv);
        /* should not happen */
        DBG_PRINT(("[mod_cgi] Error execle!\n"));
        mmp_setError(MMP_ERR_FILE);
        return -1;
    } else {
        /* the child will take care of the rest */
        /* FIXME TODO XXX remove the wait! */
        int status;
        wait(&status);
    }
    return 0;
}

