#include "mjt_headerwquality.h"

static void extp_free(struct mjt_qhead_extp_s **extp)
{
    struct mjt_qhead_extp_s *q;
    for(; *extp!=NULL; *extp = q) {
        q = (*extp)->next;
        mjt_free(&((*extp)->name));
        mjt_free2null(extp);
    }
}

void mjt_qhead_free(struct mjt_qhead_s **qhead)
{
    struct mjt_qhead_s *q;
    for(; *qhead!=NULL; *qhead = q) {
        q = (*qhead)->next;
        extp_free(&((*qhead)->extp));
        mjt_free(&((*qhead)->id));
        mjt_free2null(qhead);
    }
}

/* insert into a struct qhead_s list. We are keeping the list ordered too.
 * We're ordering by quality, from the most preferred id to least one; if the
 * entries have the same quality, the new one will be inserted after (in other
 * words, in the same order in which they appear in the original parsed
 * string) */
static int_t qhead_a_insert(struct mjt_qhead_s **qhead, struct mjt_qhead_s **q, 
                                                                bool_t alloc)
{
    struct mjt_qhead_s *p, *pp;
    if (*qhead==NULL) {
        (*q)->next = *qhead;
        *qhead = *q;
    } else {
        for (pp=NULL, p=*qhead; ; pp=p, p=p->next)
            if ((p==NULL) || ((*q)->quality > p->quality)) {
                if (pp!=NULL) pp->next = *q;
                (*q)->next = p;
                break;
            }
    }
    if (alloc==TRUE)
        if ((*q = malloc(sizeof(**q)))==NULL)
            return -1;
    return 0;
}

int_t mjt_qhead_insert(struct mjt_qhead_s **qhead, struct mjt_qhead_s *newptr)
{
    return qhead_a_insert(qhead, &newptr, FALSE);
}

void mjt_qhead_delete(struct mjt_qhead_s **qhead, struct mjt_qhead_s **delptr)
{
    struct mjt_qhead_s *p, *pp;
    if (*qhead==*delptr) {
        *qhead = (*delptr)->next;
        mjt_free2null(delptr);
    } else {
        for (pp=NULL, p=*qhead; ; pp=p, p=p->next) {
            if (p!=*delptr) continue;
            pp->next = p->next;
            extp_free(&p->extp);
            mjt_free(&p->id);
            mjt_free(&p);
            break;
        }
    }
}

/* convert a string into a "quality value". A "quality value" is a float number
 * from 0 to 1. In RFC2616 this value must have from zero to three decimal 
 * digits after the point [3.9]. We use a more relaxed approach, converting with
 * strtod() (we don't use strtof() to be more C89 compliant). If the resulting
 * value is invalid, 1.0f is taken (AKA "Better choice" :-) ). */
static float_t string2quality(char_t *str)
{
    char_t *endptr;
    float_t ret;
    errno = 0;
    ret = (float_t)strtod(str, &endptr);
    if ((errno==ERANGE) || (endptr!=NULL) || (errno!=0 && ret==0) ||
            (ret<0.0f || ret>1.0f))
        return 1.0f;
    return ret;
}

static struct mjt_qhead_extp_s *extp_insert(struct mjt_qhead_extp_s **ehead)
{
    struct mjt_qhead_extp_s *ptr;
    if ((ptr = mjt_malloc(sizeof(*ptr)))==NULL)
        return NULL;
    ptr->next = *ehead;
    *ehead = ptr;
    return ptr;
}

/* the parser! */
#define PARSE_MAIN  0
#define PARSE_KEY   1
#define PARSE_QVAL  2
#define PARSE_VAL   3
struct mjt_qhead_s *mjt_qhead_parse(char_t *head)
{
    struct mjt_qhead_s *qhead, *q;
    struct mjt_qhead_extp_s *qp;
    int_t state;
    bool_t has_q, brk;
    char_t *st;
    if (head==NULL || *head=='\0')
        return NULL;
    state = PARSE_MAIN;
    has_q = brk = FALSE;
    st = head;
    qhead = NULL;
    if ((q = mjt_malloc(sizeof(*q)))==NULL)
        return NULL;
    q->extp = NULL;
    qp = NULL;
    while(1) {
        while(((*st==' ') || (*st=='\t')) && (*st!='\0')) ++st;
        switch(*st) {
        case ';':
            *st = '\0';
            switch(state) {
            case PARSE_MAIN:
                if ((q->id = strdup(head))==NULL) goto prs_error;
                break;
            case PARSE_QVAL:
                q->quality = string2quality(head);
                has_q = TRUE;
                break;
            case PARSE_VAL:
                qp->quality = string2quality(head);
                break;
            default:
                if ( ((qp = extp_insert(&q->extp))==NULL) ||
                        ((qp->name = strdup(head))==NULL) )
                    goto prs_error;
            }
            state = PARSE_KEY;
            head = st+1;
            break;
        case '\0':
            brk = TRUE;
        case ',':
            *st = '\0';
            switch(state) {
            case PARSE_MAIN:
                if ((q->id = strdup(head))==NULL) goto prs_error;
                if (has_q==FALSE)
                    q->quality = 1.0f;
                break;
            case PARSE_QVAL:
                q->quality = string2quality(head);
                break;
            case PARSE_VAL:
                if (qp==NULL) goto prs_error;
                qp->quality = string2quality(head);
                break;
            default:
                if ( ((qp = extp_insert(&q->extp))==NULL) ||
                        ((qp->name = strdup(head))==NULL) )
                    goto prs_error;
            }
            if (brk==FALSE) {
                if (qhead_a_insert(&qhead, &q, TRUE)!=0) goto prs_error;
                q->extp = NULL;
                state = PARSE_MAIN;
                has_q = FALSE;
                head = st+1;
            } else {
                if (qhead_a_insert(&qhead, &q, FALSE)!=0) goto prs_error;
                goto done;
            }
            break;
        case '=':
            *st = '\0';
            if (state==PARSE_KEY) {
                if (!strcmp(head, "q")) {
                    state = PARSE_QVAL;
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
    mjt_qhead_free(&qhead);
    return NULL;
}

