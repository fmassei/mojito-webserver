#include "header_w_quality.h"

/* free an extp list */
static void qhead_extp_free(struct extp_s *extp)
{
    struct extp_s *ep, *eq;
    for (ep=extp; ep!=NULL; ep=eq) {
        eq = ep->next;
        free(ep->name);
        free(ep);
    }
}

/* frees a qhead list */
void qhead_free(struct qhead_s *qhead)
{
    struct qhead_s *q;
    for (q=qhead; q!=NULL; ) {
        qhead = q->next;
        qhead_extp_free(q->extp);
        free(q->id);
        free(q);
        q = qhead;
    }
}

/* insert into a struct qhead_s list. We are keeping the list ordered too.
 * We're ordering by quality, from the most preferred id to least one; if the
 * entries have the same quality, the new one will be inserted after (in other
 * words, in the same order in which they appear in the original parsed
 * string) */
static int qhead_a_insert(struct qhead_s **qhead, struct qhead_s **q, int alloc)
{
    struct qhead_s *p, *pp;
    if (*qhead==NULL) {
        (*q)->next = *qhead;
        *qhead = *q;
    } else {
        for (pp=NULL, p=*qhead; ; pp=p, p=p->next) {
            if ((p==NULL) || ((*q)->quality > p->quality)) {
                if (pp!=NULL) pp->next = *q;
                (*q)->next = p;
                break;
            }
        }
    }
    if (alloc)
        if ((*q = malloc(sizeof(struct qhead_s)))==NULL)
            return -1;
    return 0;
}

/* insert in the queue */
int qhead_insert(struct qhead_s **qhead, struct qhead_s *p)
{
    return qhead_a_insert(qhead, &p, 0);
}

/* drop an entry. */
void qhead_delete(struct qhead_s **qhead, struct qhead_s *e2d)
{
    struct qhead_s *p, *pp;
    if (*qhead==e2d) {
        *qhead = e2d->next;
        free(e2d);
    } else {
        for (pp=NULL, p=*qhead; ; pp=p, p=p->next) {
            if (p!=e2d) continue;
            pp->next = p->next;
            qhead_extp_free(p->extp);
            free(p->id);
            free(p);
            break;
        }
    }
}

/* convert a string into a "quality value". A "quality value" is a float number
 * from 0 to 1. In RFC2616 this value must have from zero to three decimal 
 * digits after the point [3.9]. We use a more relaxed approach, converting with
 * strtod() (we don't use strtof() to be more C89 compliant). If the resulting
 * value is invalid, 1.0f is taken (AKA "Better choice" :-) ). */
static float string2quality(char *str)
{
    char *endptr;
    float ret;
    errno = 0;
    ret = (float)strtod(str, &endptr);
    if ((errno==ERANGE) || (endptr!=NULL) || (errno!=0 && ret==0) || 
            (ret<0.0f || ret>1.0f))
        return 1.0f;
    return ret;
}

/* insert on the top of the extp_s list */
static struct extp_s *extp_insert(struct extp_s **ehead)
{
    struct extp_s *qp;
    if ((qp = malloc(sizeof(*qp)))==NULL)
        return NULL;
    qp->next = *ehead;
    *ehead = qp;
    return qp;
}

#define PARSE_MAIN  0
#define PARSE_KEY   1
#define PARSE_QVAL  2
#define PARSE_VAL   3
struct qhead_s *qhead_parse(char *head)
{
    struct qhead_s *qhead, *q;
    struct extp_s *qp;
    int rq, has_q, brk;
    char *st;
    if (head==NULL || *head=='\0')
        return NULL;
    rq = PARSE_MAIN;
    has_q = brk = 0;
    st = head;
    qhead = NULL;
    if ((q = malloc(sizeof(*q)))==NULL)
        return NULL;
    q->extp = NULL;
    qp = NULL;
    while(1) {
        while(((*st==' ') || (*st=='\t')) && (*st!='\0')) ++st;
        switch(*st) {
        case ';':
            *st = '\0';
            if (rq==PARSE_MAIN) {
                if ((q->id = strdup(head))==NULL) goto prs_error;
            } else if (rq==PARSE_QVAL) {
                q->quality = string2quality(head);
                has_q = 1;
            } else if (rq==PARSE_VAL) {
                qp->quality = string2quality(head);
            } else {
                if ( ((qp = extp_insert(&q->extp))==NULL) ||
                        ((qp->name = strdup(head))==NULL) )
                    goto prs_error;
            }
            rq = PARSE_KEY;
            head = st+1;
            break;
        case '\0':
            brk = 1;
        case ',':
            *st = '\0';
            if (rq==PARSE_MAIN) {
                if ((q->id = strdup(head))==NULL) goto prs_error;
                if (has_q == 0)
                    q->quality = 1.0f;
            } else if (rq==PARSE_QVAL) {
                q->quality = string2quality(head);
            } else if (rq==PARSE_VAL) {
                if (qp==NULL) goto prs_error;
                qp->quality = string2quality(head);
            } else {
                if ( ((qp = extp_insert(&q->extp))==NULL) ||
                        ((qp->name = strdup(head))==NULL) )
                    goto prs_error;
            }
            if (brk==0) {
                if (qhead_a_insert(&qhead, &q, 1)!=0) goto prs_error;
                q->extp = NULL;
                rq = PARSE_MAIN;
                has_q = 0;
                head = st+1;
            } else {
                if (qhead_a_insert(&qhead, &q, 0)!=0) goto prs_error;
                goto done;
            }
            break;
        case '=':
            *st = '\0';
            if (rq==PARSE_KEY) {
                if (!strcmp(head, "q")) {
                    rq = PARSE_QVAL;
                } else {
                    if ( ((qp = extp_insert(&q->extp))==NULL) ||
                            ((qp->name = strdup(head))==NULL) )
                        goto prs_error;
                }
                head = st+1;
            }
            break;
        }
        ++st;
    }
done:
    return qhead;
prs_error:
    qhead_free(qhead);
    return NULL;
}

