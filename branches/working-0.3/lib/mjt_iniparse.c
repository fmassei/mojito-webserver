#include "mjt_iniparse.h"
#include "mjt_alloc.h"
#include "mjt_kvlist.h"

#define PARSESTATE_BLINE 1
#define PARSESTATE_NAME  2
#define PARSESTATE_VALUE 3
#define PARSESTATE_ERROR 4

#define STR_ERROR_FORMAT "error parsing file %s, line %d: %s (code %d)"
static char_t inierror[0xff] = {'\0'};

static void setinierror_reason(const char_t *fname, int_t line,
                                const char_t *reason, int_t reason_code)
{
    snprintf(inierror, sizeof(inierror), STR_ERROR_FORMAT, fname, line,
                                                        reason, reason_code);
}

static void setinierror(const char_t *fname, int_t line)
{
    setinierror_reason(fname, line, strerror(errno), errno);
}

char_t *mjt_inigeterror(void)
{
    return inierror;
}

static void inisection_free(struct inisection_s **ptr)
{
    if (ptr==NULL || *ptr==NULL)
        return;
    mjt_free(&(*ptr)->name);
    mjt_kvlist_destroy(&(*ptr)->params);
    mjt_free2null(ptr);
}

void mjt_inifree(struct inisection_s **ptr)
{
    struct inisection_s *q;
    if (ptr==NULL || *ptr==NULL)
        return;
    for(; *ptr!=NULL; *ptr=q) {
        q = (*ptr)->next;
        inisection_free(ptr);
    }
}

static struct inisection_s *inisection_create(char_t *name)
{
    struct inisection_s *ptr;
    if ((ptr = mjt_malloc(sizeof(*ptr)))==NULL)
        return NULL;
    ptr->name = NULL;
    ptr->params = NULL;
    ptr->next = NULL;
    if (name!=NULL)
        if ((ptr->name = strdup(name))==NULL) {
            inisection_free(&ptr);
            return NULL;
        }
    return ptr;
}

static int_t inisection_addparam(struct inisection_s *s, char_t *k, char_t *v)
{
    if (mjt_kvlist_insert(&(s->params), k, v, 0)<0)
        return -1;
    return 0;
}

static char_t *string_trim(char_t *str)
{
    int_t i;
    while(*str==' ' || *str=='"') ++str;
    for (i=strlen(str)-1; (str[i]==' ' || str[i]=='"') && (i>=0); --i)
        str[i] = '\0';
    return str;
}

static int_t string_normalize(char_t **str)
{
    if (strlen(*str)<=0) return -1;
    *str = string_trim(*str);
    if (strlen(*str)<=0) return -1;
    return 0;
}

struct inisection_s *mjt_iniparse(const char_t *fname)
{
    struct inisection_s *ret = NULL, *p;
    struct stat sb;
    char_t *addr, c, *bname, *bval;
    int_t fd, i, state, line;
    sizet_t len;
    line = 0;
    bzero(&sb, sizeof(sb));
    if (    (stat(fname, &sb)<0) ||
            ((fd = open(fname, O_RDONLY))<0) ||
            ((addr = mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE,
                            fd, 0))==MAP_FAILED) ) {
        /* on linux, this could fail with EINVAL if sb.st_size is zero */
        setinierror(fname, line);
        return NULL;
    }
    len = sb.st_size;
    state = PARSESTATE_BLINE;
    for (i=0; i<len; ++i) {
        c = *(addr+i);
        switch(state) {
        case PARSESTATE_BLINE:
            if (c==';') {
                for(;((i<len)&&(*(addr+i)!='\n')); ++i) ;
                if (addr[i]=='\n') ++line;
                continue;
            } else if (c=='\n') {
                ++line;
                continue;
            } else if (c=='[') {
                bname = addr+(++i);
                for(;((i<len)&&(*(addr+i)!='\n')&&((*addr+i)!=']')); ++i) ;
                if (addr[i]=='\n') ++line;
                addr[i] = '\0';
                if ((p = inisection_create(bname))==NULL) {
                    setinierror(fname, line);
                    state = PARSESTATE_ERROR;
                    goto done;
                }
                p->next = ret;
                ret = p;
                for(;((i<len)&&(*(addr+i)!='\n')); ++i) ;
                if (addr[i]=='\n') ++line;
                continue;
            }
            bname = addr+i;
            state = PARSESTATE_NAME;
            break;
        case PARSESTATE_NAME:
            if (c=='=') {
                addr[i] = '\0';
                bval = addr+(++i);
                state = PARSESTATE_VALUE;
            }
            break;
        case PARSESTATE_VALUE:
            if (c=='\n') {
                ++line;
                addr[i] = '\0';
                if (ret==NULL) {
                    setinierror_reason(fname, line, "Missing main section", 0);
                    state = PARSESTATE_ERROR;
                    goto done;
                }
                if (    (string_normalize(&bname)<0) ||
                        (string_normalize(&bval)<0) ||
                        (inisection_addparam(ret, bname, bval)<0) ) {
                    setinierror(fname, line);
                    state = PARSESTATE_ERROR;
                    goto done;
                }
                state = PARSESTATE_BLINE;
            }
            break;
        }
    }
done:
    munmap(addr, len);
    close(fd);
    if (state!=PARSESTATE_BLINE) {
        mjt_inifree(&ret);
        return NULL;
    }
    return ret;
}

