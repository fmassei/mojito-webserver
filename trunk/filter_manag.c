#include "filter_manag.h"

/* find a filter by its id */
static struct module_filter_s *filter_findbyid(char *id)
{
    extern struct module_filter_s *filter_modules;
    struct module_filter_s *f;
    for (f=filter_modules; f!=NULL; f=f->next)
        if (!strcmp(f->name, id))
            return f;
    return NULL;
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
    extern struct module_filter_s *filter_modules;
    struct qhead_s *p, *q;
    struct module_filter_s *f;
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
        for (f=filter_modules; f!=NULL; f=f->next) {
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
struct module_filter_s *filter_findfilter(struct qhead_s *qhead)
{
    extern struct module_filter_s *filter_modules;
    struct qhead_s *p;
    struct module_filter_s *f = NULL;
    for (p=qhead; p!=NULL; p=p->next)
        for (f=filter_modules; f!=NULL; f=f->next) {
            if (!strcmp(f->name, p->id))
                return f;
        }
    return NULL;
}

