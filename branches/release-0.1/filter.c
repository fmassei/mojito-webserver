#include "filter.h"

/* our filters */
struct filter_s *filters = NULL;
/* we keep a pointer to the identity filter. There is an entry in the filter
 * list too, but we need a direct link to this one due to its importance. It's
 * mainly used when outputting cached payloads. */
struct filter_s *ident_filter = NULL;

/* find a filter by its id */
static struct filter_s *filter_findbyid(char *id)
{
    struct filter_s *f;
    for (f=filters; f!=NULL; f=f->next)
        if (!strcmp(f->name, id))
            return f;
    return NULL;
}

/* initialize global filter struct */
int filter_init()
{
    if (filters!=NULL)
        filter_free();
    filters = NULL;
    return 0;
}

/* add a filter to the filters queue */
struct filter_s *filter_register(char *name, filter_ft worker,
                                                        filter_len_ft prelen)
{
    struct filter_s *q;
    if (name==NULL)
        return NULL;
    if ((q = malloc(sizeof(*q)))==NULL)
        return NULL;
    if ((q->name = strdup(name))==NULL) {
        free(q);
        return NULL;
    }
    q->worker = worker;
    q->prelength = prelen;
    q->next = filters;
    filters = q;
    return q;
}

/* for filter queues we have a few rules:
 * - identity filter (if not present) has to be considered present. We choose
 *   quality = 1.000
 * - if a '*' is present, it marks all the available filters (identity too)
 *
 * TODO This sanitize function is a cpu-cycle eater! Too much cycles!
 * TODO check for the case of empty list at the end of the function: maybe we
 *      deal with it later, but I'm not sure. */
int filter_sanitize_queue(struct qhead_s **qhead)
{
    struct qhead_s *p, *q;
    struct filter_s *f;
    float rq;
    /* check for identity */
    for (p=*qhead; p!=NULL; p=p->next)
        if (!strcmp(p->id, "identity"))
            break;
    /* no identity? add it */
    if (p==NULL) {
        if ((p = malloc(sizeof(*p)))==NULL)
            return -1;
        if ((p->id = strdup("identity"))==NULL) {
            free(p);
            return -1;
        }
        p->quality = 1.0f;
        if (qhead_insert(qhead, p)<0) {
            free(p->id);
            free(p);
            return -1;
        }
    }
    /* check for '*' */
    for (p=*qhead; p!=NULL; p=p->next) {
        if (strcmp(p->id, "*"))
            continue;
        /* ok - we've got a '*'. Drop the entry and, for each filter we've
         * got that is not present, do an insert with the '*' quality */
        rq = p->quality;
        qhead_delete(qhead, p);
        for (f=filters; f!=NULL; f=f->next) {
            for (q=*qhead; q!=NULL; q=q->next)
                if (!strcmp(q->id, f->name))
                    break;
            if (q==NULL) {
                if ( ((q = malloc(sizeof(*q)))==NULL) ||
                        ((q->id = strdup(f->name))==NULL) )
                    return -1; /* aww come on... just let it explode */
                q->quality = rq;
                q->extp = NULL;
                if (qhead_insert(qhead, q)!=0)
                    return -1;
            }
        }
        break;
    }
    /* drop all the filters that we can't handle */
    for (p=*qhead; p!=NULL; p=p->next)
        if (filter_findbyid(p->id)==NULL)
            qhead_delete(qhead, p);
    return 0;
}

int filter_is_present(struct qhead_s *qhead, char *id)
{
    struct qhead_s *p;
    for (p=qhead; p!=NULL; p=p->next)
        if (!strcmp(p->id, id))
            return 1;
    return 0;
}

/* scroll the qhead struct trying to find a usable filter based on user
 * preferences (RFC2616-14.3). The qhead list should be ordered. */
struct filter_s *filter_findfilter(struct qhead_s *qhead)
{
    struct qhead_s *p;
    struct filter_s *f = NULL;
    for (p=qhead; p!=NULL; p=p->next)
        for (f=filters; f!=NULL; f=f->next) {
            if (!strcmp(f->name, p->id))
                return f;
        }
    return NULL;
}

/* free all the filters memory */
void filter_free()
{
    struct filter_s *q;
    for (q=filters; q!=NULL;) {
        filters = q->next;
        free(q->name);
        free(q);
        q = filters;
    }
}

