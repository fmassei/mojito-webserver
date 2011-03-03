#include "header_w_quality.h"

/* free an extp */
static void extp_free(t_extp_s **extp)
{
    if (extp==NULL || *extp==NULL) return;
    if ((*extp)->name!=NULL)
        xfree((*extp)->name);
    xfree(*extp);
    *extp = NULL;
}
static void extp_free_v(void **ptr)
{
    extp_free((t_extp_s**)ptr);
}

/* free a qhead */
static void qhead_free(t_qhead_s **qhead)
{
    if (qhead==NULL || *qhead==NULL) return;
    if ((*qhead)->id!=NULL)
        xfree((*qhead)->id);
    mmp_list_delete_withdata(&(*qhead)->extp_list, extp_free_v);
    xfree(*qhead);
    *qhead = NULL;
}
static void qhead_free_v(void **ptr)
{
    qhead_free((t_qhead_s**)ptr);
}
void qhead_list_destroy(t_qhead_list_s **qhead_list)
{
    mmp_list_delete_withdata(qhead_list, qhead_free_v);
}

/* qhead comparer functions */
static int qhead_compare_rev(const t_qhead_s *q1, const t_qhead_s *q2)
{
    if (q1->quality > q2->quality)
        return 1;
    else if (q1->quality < q2->quality)
        return -1;
    return 0;
}
static int qhead_compare_rev_v(const void *p1, const void *p2)
{
    return qhead_compare_rev((t_qhead_s*)p1, (t_qhead_s*)p2);
}

/* insert into a struct qhead_s list. We are keeping the list ordered too.
 * We're ordering by quality, from the most preferred id to least one; if the
 * entries have the same quality, the new one will be inserted after (in other
 * words, in the same order in which they appear in the original parsed
 * string) */
ret_t qhead_list_insert(t_qhead_list_s *qhead_list, t_qhead_s *qhead)
{
    if (qhead_list==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    return mmp_list_add_data_sorted(qhead_list, qhead, qhead_compare_rev_v);
}

/* drop an entry. */
void qhead_list_delete(t_qhead_list_s *qhead_list, t_qhead_s **e2d)
{
    if (e2d==NULL) return;
    mmp_list_del_elem_by_data(qhead_list, *e2d);
    mmp_list_delete_withdata(&(*e2d)->extp_list, extp_free_v);
    xfree((*e2d)->id);
    xfree(*e2d);
    *e2d = NULL;
}

t_qhead_s *qhead_create(const char *id)
{
    t_qhead_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    if (id!=NULL) {
        if ((ret->id = xstrdup(id))==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            xfree(ret);
            return NULL;
        }
    } else {
        ret->id = NULL;
    }
    ret->quality = 0;
    ret->extp_list = NULL;
    return ret;
}

void qhead_destroy(t_qhead_s **qhead)
{
    qhead_free(qhead);
}


/* convert a string into a "quality value". A "quality value" is a float number
 * from 0 to 1. In RFC2616 this value must have from zero to three decimal 
 * digits after the point [3.9]. We use a more relaxed approach, converting with
 * strtod() (we don't use strtof() to be more C89 compliant). If the resulting
 * value is invalid, 1.0f is taken (AKA "Better choice" :-) ). */
static float string2quality(const char *str)
{
    char *endptr = NULL;
    float ret;
    errno = 0;
    ret = (float)strtod(str, &endptr);
    if ((errno==ERANGE) || (errno!=0 && ret==0) || (ret<0.0f && ret>1.0f))
        return 1.0f;
    return ret;
}

/* create a new extp */
static t_extp_s *extp_create_n(const char *name, size_t n)
{
    t_extp_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    if (name==NULL) {
        ret->name = NULL;
    } else {
        if ((ret->name = xstrdupn(name, n))==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            xfree(ret);
            return NULL;
        }
    }
    return ret;
}

/* insert an extp into a qhead */
static ret_t qhead_insert_extp(t_qhead_s *parent, t_extp_s *extp)
{
    if (parent!=NULL) {
        if (parent->extp_list==NULL)
            if ((parent->extp_list = mmp_list_create())==NULL) {
                mmp_setError(MMP_ERR_GENERIC);
                return MMP_ERR_GENERIC;
            }
        if (mmp_list_add_data(parent->extp_list, extp)!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return MMP_ERR_GENERIC;
        }
    }
    return MMP_ERR_OK;
}

#define PARSE_MAIN  0
#define PARSE_KEY   1
#define PARSE_QVAL  2
#define PARSE_VAL   3
t_qhead_list_s *qhead_list_parse(const char *head)
{
    t_qhead_list_s *ret = NULL;
    t_extp_s *e = NULL;
    t_qhead_s *q = NULL;
    int rq, has_q, brk;
    const char *st;
    if (head==NULL || *head=='\0')
        return NULL;
    if ((ret = mmp_list_create())==NULL) {
        mmp_setError(MMP_ERR_GENERIC);
        return NULL;
    }
    rq = PARSE_MAIN;
    has_q = brk = 0;
    st = head;
    if ((q = xmalloc(sizeof(*q)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        goto prs_error;
    }
    q->extp_list = NULL;
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4127)
    while(1) {
#pragma warning(pop)
#else
    while(1) {
#endif
        while(((*st==' ') || (*st=='\t')) && (*st!='\0')) ++st;
        switch(*st) {
        case ';':
            if (rq==PARSE_MAIN) {
                if ((q->id = xstrdupn(head, st-head))==NULL)
                    goto prs_error;
            } else if (rq==PARSE_QVAL) {
                q->quality = string2quality(head);
                has_q = 1;
            } else if (rq==PARSE_VAL) {
                e->quality = string2quality(head);
            } else {
                if (    ((e = extp_create_n(head, st-head))==NULL) ||
                        (qhead_insert_extp(q, e)!=MMP_ERR_OK) )
                    goto prs_error;
            }
            rq = PARSE_KEY;
            head = st+1;
            break;
        case '\0':
            brk = 1;
        case ',':
            if (rq==PARSE_MAIN) {
                if ((q->id = xstrdupn(head, st-head))==NULL)
                    goto prs_error;
                if (has_q == 0)
                    q->quality = 1.0f;
            } else if (rq==PARSE_QVAL) {
                q->quality = string2quality(head);
            } else if (rq==PARSE_VAL) {
                if (e==NULL)
                    goto prs_error;
                e->quality = string2quality(head);
            } else {
                if (    ((e = extp_create_n(head, st-head))==NULL) ||
                        (qhead_insert_extp(q, e)!=MMP_ERR_OK) )
                    goto prs_error;
            }
            if (brk==0) {
                if (qhead_list_insert(ret, q)!=MMP_ERR_OK)
                    goto prs_error;
                if ((q = xmalloc(sizeof(*q)))==NULL) {
                    mmp_setError(MMP_ERR_ENOMEM);
                    goto prs_error;
                }
                q->extp_list = NULL;
                rq = PARSE_MAIN;
                has_q = 0;
                head = st+1;
            } else {
                if (qhead_list_insert(ret, q)!=MMP_ERR_OK)
                    goto prs_error;
                goto done;
            }
            break;
        case '=':
            if (rq==PARSE_KEY) {
                if (!strncmp(head, "q", 1)) {
                    rq = PARSE_QVAL;
                } else {
                    if (    ((e = extp_create_n(head, st-head))==NULL) ||
                            (qhead_insert_extp(q, e)!=MMP_ERR_OK) )
                        goto prs_error;
                }
                head = st+1;
            }
            break;
        }
        ++st;
    }
done:
    return ret;
prs_error:
    if (e!=NULL) extp_free(&e);
    if (q!=NULL) qhead_free(&q);
    if (ret!=NULL) qhead_list_destroy(&ret);
    return NULL;
}

