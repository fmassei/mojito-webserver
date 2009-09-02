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

#include "response.h"

/* protocol strings */
static const char HTTP10[] = "HTTP/1.0 ";
static const char HTTP11[] = "HTTP/1.1 ";
/* some status responses */
static const char RESP200[] = "200 OK\r\n";
static const char RESP404[] = "404 Not Found\r\n";
static const char RESP406[] = "406 Not Acceptable\r\n";
static const char RESP500[] = "500 Internal Server Error\r\n";
static const char RESP501[] = "501 Not Implemented\r\n";
/* buffer for responses. Just make them fixed and static */
static char res[0xff];
static char buf[0xff];
/* final filter(s) */
static struct module_filter_s *filter;

static char *page;
char *query_string;

/* extract url and query string from the uri */
static void strip_uri(const char *uri)
{
    int p;
    /* divide by host and query_string */
    if ((query_string = index(uri, '?'))==NULL) { /* no qs */
        page = (char*)uri;
    } else {
        /* strip the '?' character */
        ++query_string; 
        /* copy and terminate the 'page' string */
        p = (int)(query_string - uri - 1);
        page = malloc(p+1);
        memcpy(page, uri, p);
        page[p] = '\0';
    }
}

/* send the generic response header */
static void send_head(const char *head)
{
    extern fparams_st params;
    extern int proto_version;
    res[0] = '\0';
    /* RFC2145 - conservative approach */
    if (proto_version==P_HTTP_11)
        strcat(res, HTTP11);
    else strcat(res, HTTP10);
    strcat(res, head);
    sprintf(buf, "Date: %s\r\n", time_1123_format(time(NULL)));
    strcat(res, buf);
    sprintf(buf, "Server: %s\r\n", params.server_meta);
    strcat(res, buf);
    /*strcat(res, "Connection: close\r\n");*/
}

/* send the content-length */
static void send_contentlength(long len)
{
    sprintf(buf, "Content-Length: %lu\r\n", len);
    strcat(res, buf);
}

/* send the content-type */
static void send_contenttype(char *name)
{
    sprintf(buf, "Content-Type: ");
    strcat(buf, name);
    strcat(buf, "\r\n");
    strcat(res, buf);
}

/* send the filter content-encoding */
static void send_filter_encoding(char *name)
{
    /* very very ugly. But we must have an expection somewhere */
    if (!strcmp(name, "identity"))
        return;
    strcat(res, "Content-Encoding: ");
    strcat(res, name);
    strcat(res, "\r\n");
}

/* send the prepared header */
static void send_endhead(int sock)
{
    strcat(res, "\r\n");
    write(sock, res, strlen(res));
}

/* send generic status strings */
void push_200(int sock)
{
    send_head(RESP200);
    write(sock, res, strlen(res));
}
void send_404(int sock)
{
    send_head(RESP404);
    send_contentlength(0);
    send_endhead(sock);
}
void send_406(int sock)
{
    send_head(RESP406);
    send_contentlength(0);
    send_endhead(sock);
}
void send_500(int sock)
{
    send_head(RESP500);
    send_contentlength(0);
    send_endhead(sock);
}
void send_501(int sock)
{
    send_head(RESP501);
    send_contentlength(0);
    send_endhead(sock);
}

/* check if we can deal the request header */
static int check_method()
{
    extern int method;
    if (method!=M_GET && method!=M_POST && method!=M_HEAD) {
        return -1;
    }
    return 0;
}

#ifndef NOCACHE
/* send a cached file */
static void send_cached_file(struct cache_entry_s *cache_file, int sock)
{
    extern struct module_filter_s *ident_filter;
    extern int method;
    unsigned char *addr;
    struct stat sb;
    int clen;
    int fd;
    memset(&sb, 0, sizeof(sb));
    stat(cache_file->fname, &sb);
    if ((fd = open(cache_file->fname, 0))<0) {
        send_head(RESP500);
        return;
    }
    send_head(RESP200);
    if ((clen = ident_filter->prelen(&sb))>=0)
        send_contentlength(clen);
    send_filter_encoding(cache_file->filter_id);
    send_contenttype(cache_file->content_type);
    send_endhead(sock);
    if (method==M_HEAD)
        return;
    addr = mmap(NULL, clen, PROT_READ, MAP_PRIVATE, fd, 0);
    ident_filter->compress(addr, sock, clen);
}
#endif

/* send the response */
void send_file(int sock, const char *uri)
{
    extern fparams_st params;
    extern int method;
    extern struct qhead_s *accept_encoding;
    extern int keeping_alive;
    struct stat sb;
    unsigned char *addr;
    char *filename;
    long clen;
    int fd;
#ifndef NOCACHE
    struct cache_entry_s *cache_file;
    struct qhead_s *aep;
    int cfd;
#endif

    if (check_method()!=0) {
        send_501(sock);
        return;
    }
#ifndef NOCACHE
    /* check if we have a file in cache */
    for (aep=accept_encoding; aep!=NULL; aep=aep->next) {
        if (!strcmp(aep->id, "identity"))
            continue;
        if ((cache_file = cache_lookup(uri, aep->id))!=NULL) {
            send_cached_file(cache_file, sock);
            return;
        }
    }
    DEBUG_LOG((LOG_DEBUG, "Normal sending"));
#endif
    /* no cache? build one */
    strip_uri(uri);
    if ((filename = malloc(strlen(params.http_root)+strlen(page)+1))==NULL) {
        send_500(sock);
        return;
    }    
    sprintf(filename, "%s%s", params.http_root, page);
redo:
    memset(&sb, 0, sizeof(sb));
    stat(filename, &sb);
    if ((sb.st_mode&S_IFMT)!=S_IFREG) {
        if ((sb.st_mode&S_IFMT)==S_IFDIR) {
            if ((filename = realloc(filename, strlen(filename)+
                                        strlen(params.default_page)+1))==NULL) {
                send_500(sock);
                return;
            }
            sprintf(filename, "%s%s", filename, params.default_page);
            goto redo;
        }
        send_404(sock);
        return;
    }
    if (access(filename, X_OK)==0) {
        cgi_run(filename, sock);
        /* if here the cgi failed. Send a 500 to the client */
        send_500(sock);
    }
    filter = filter_findfilter(accept_encoding);
    if (filter==NULL) {
        send_406(sock);
        return;
    }
    if ((fd = open(filename, O_RDONLY))<=0) {
        send_404(sock);
        return;
    }
    send_head(RESP200);
    if ((clen = filter->prel(&sb))>=0) {
        send_contentlength(clen);
    } else {
        /* no length? no party. We can't keep the connection alive */
        keeping_alive = 0;
    }
    send_filter_encoding(filter->name);
    send_contenttype(mime_gettype(filename));
    send_endhead(sock);
    if (method==M_HEAD)
        return;
    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    filter->compress(addr, sock, sb.st_size);
    /* add in cache (only if filter != identity!) */
    if (!strcmp(filter->name, "identity"))
        return;
#ifndef NOCACHE
    if ((cfd = cache_create_file(uri, filter->name, mime_gettype(filename)))>=0)
        filter->compress(addr, cfd, sb.st_size);
#endif
}

