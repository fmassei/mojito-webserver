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

#include "cache.h"
#include "linear_hashtab.h"
#include "../modules.h"
#include "../../response.h"

static struct lh_s *page_cache;
static struct plist_s *params;
static char *cache_dir;

static char *buildkey(const char *URI, const char *filter_id)
{
    char *key;
    if ((key = malloc(strlen(URI)+strlen(filter_id)+1))==NULL)
        return NULL;
    strcpy(key, URI);
    strcat(key, filter_id);
    return key;
}

/* set global parameters */
static int cache_set_parameters(struct plist_s *pars)
{
    params = pars;
    if (pars==NULL)
        return MOD_CRIT;
    if ((cache_dir = plist_search(params, "cache_dir"))==NULL)
        return MOD_CRIT;
    if (cache_dir[strlen(cache_dir)-1]=='/')
        cache_dir[strlen(cache_dir)-1] = '\0';
    return MOD_OK;
}

/* search the cache for the URI and filter */
static struct cache_entry_s *cache_lookup(const char *URI,
                                                        const char *filter_id)
{
    struct cache_entry_s *ret;
    struct entry_s *p;
    char *key;
    key = buildkey(URI, filter_id);
    DEBUG_LOG((LOG_DEBUG, "cache_lookup: %s %s", URI, filter_id));
    if ((p = lhlookup(page_cache, key))==NULL) {
        DEBUG_LOG((LOG_DEBUG, "Not found in cache."));
        return NULL;
    }
    DEBUG_LOG((LOG_DEBUG, "Found in cache."));
    if (p->b!=0) {
        DEBUG_LOG((LOG_DEBUG, "Entry busy!"));
        return NULL; /* busy */
    }
    ret = malloc(sizeof(*ret));
    ret->URI = (char*)URI;
    ret->filter_id = (char*)filter_id;
    ret->fname = p->data;
    for(ret->content_type=ret->fname; *(ret->content_type)!='\0';
                ++ret->content_type);
    ++ret->content_type;
    DEBUG_LOG((LOG_DEBUG, "%s %s %s %s.", ret->URI, ret->fname,
                                            ret->filter_id, ret->content_type));
    return ret;
}

/* install an URI into the cache */
static int cache_install(const char *URI, char *fname,
                                            char *filter_id, char *content_type)
{
    unsigned int dlen, l_fname, l_contenttype;
    unsigned char *buf;
    char *key;
    if (URI==NULL || fname==NULL || filter_id==NULL)
        return -1;
    key = buildkey(URI, filter_id);
    l_fname = strlen(fname);
    l_contenttype = strlen(content_type);
    dlen = l_fname + 1 + l_contenttype + 1;
    if ((buf = malloc(dlen))==NULL)
        return -1;
    DEBUG_LOG((LOG_DEBUG, "Cache installing %s %s %s.", fname, filter_id,
                                                          content_type));
    memcpy(buf, fname, l_fname+1);
    memcpy(buf+l_fname+1, content_type, l_contenttype+1);
    if (lhinstall(page_cache, key, buf, dlen)<0)
        return -1;
    return 0;
}

/* create a cache file and return its file description */
static int _cache_create_file(const char *URI, char *filter_id,
                                                            char *content_type)
{
    static char cache_file[2049];
    int fd;
    snprintf(cache_file, 2049, "%s/mojito-cache.XXXXXX", cache_dir);
    if ((fd = mkstemp(cache_file))==-1) {
        perror("mkstemp");
        return -1;
    }
    if (cache_install(URI, cache_file, filter_id, content_type)<0) {
        close(fd);
        unlink(cache_file);
        return -1;
    }
    return fd;
}

static int cache_init(void)
{
    if ((page_cache = lhcreate(65536))==NULL)
        return -1;
    return MOD_OK;
}

static int cache_fini(void)
{
    lhdestroy(page_cache);
    return MOD_OK;
}

/* send a cached file */
static void send_cached_file(struct cache_entry_s *cache_file, int sock,
                                                        struct request_s *req)
{
    unsigned char *addr;
    struct stat sb;
    int clen;
    int fd;
    DEBUG_LOG((LOG_DEBUG, "Sending cached."));
    memset(&sb, 0, sizeof(sb));
    stat(cache_file->fname, &sb);
    if ((fd = open(cache_file->fname, 0))<0) {
        header_push_code(HRESP_500);
        return;
    }
    header_push_code(HRESP_200);
    header_push_contentlength(sb.st_size);
    header_push_contentencoding(cache_file->filter_id);
    header_push_contenttype(cache_file->content_type);
    header_send(sock);
    if (req->method==M_HEAD)
        return;
    clen = sb.st_size;
    addr = mmap(NULL, clen, PROT_READ, MAP_PRIVATE, fd, 0);
    write(sock, addr, clen);
}

static int _on_presend(int sock, struct request_s *req)
{
    struct cache_entry_s *cache_file;
    struct qhead_s *aep;
    for (aep = req->header.accept_encoding; aep!=NULL; aep=aep->next) {
        if (!strcmp(aep->id, "identity"))
            continue;
        if ((cache_file = cache_lookup(req->uri, aep->id))!=NULL) {
            send_cached_file(cache_file, sock, req);
            return MOD_ALLDONE;
        }
    }
    return MOD_OK;
}

static int _on_postsend(struct request_s *req,
                                        char *mime, void *addr, struct stat *sb)
{
    extern struct module_s *ch_filter;
    int cfd;
    if (!strcmp(ch_filter->name, "identify"))
        return MOD_OK;
    /* FIXME adjust this ch_filter */
    if ((cfd = _cache_create_file(req->uri, ch_filter->name, mime))<0)
        return MOD_CRIT;
    if (on_send(addr, cfd, sb)!=0)
        return MOD_CRIT;
    return MOD_OK;
}

/* define MODULE_STATIC in the Makefile! */
#ifdef MODULE_STATIC
struct module_s *mod_cacheshm_getmodule()
#else
struct module_s *getmodule()
#endif
{
    struct module_s *p;
    if ((p = malloc(sizeof(*p)))==NULL)
        return NULL;
    p->name = "cacheshm";
    p->set_params = cache_set_parameters;
    p->init = cache_init;
    p->fini = cache_fini;
    p->can_run = NULL;
    p->on_accept = NULL;
    p->on_presend = _on_presend;
    p->on_prehead = NULL;
    p->on_send = NULL;
    p->on_postsend = _on_postsend;
    p->will_run = 1;
    p->category = MODCAT_CACHE;
    p->next = p->prev = NULL;
    return p;
}

