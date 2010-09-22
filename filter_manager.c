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
#include "filter_manager.h"

/* find a filter by its id */
static t_module_s *filter_findbyid(char *id)
{
    t_mmp_listelem_s *p;
    t_module_list_s *filters;
    t_module_s *mod;
    filters = module_getfilterlist();
    for (p=filters->head; p!=NULL; p=p->next) {
        mod = (t_module_s*)p->data;
        if (mod==NULL) continue;
        if (!strcmp(mod->name, id))
            return mod;
    }
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
ret_t filter_sanitize_queue(t_qhead_list_s *qhead)
{
    t_mmp_listelem_s *p, *q, *k;
    t_module_list_s *filters;
    t_module_s *filt;
    ret_t ret;
    /*struct qhead_s *p, *q;
    struct module_filter_s *f;*/
    float rq;
    t_qhead_s *qh, *qh2;
    filters = module_getfilterlist();
    /* check for identity */
    qh = NULL;
    for (p=qhead->head; p!=NULL; p=p->next) {
        qh = (t_qhead_s*)p;
        if (qh==NULL) continue;
        if (!strcmp(qh->id, "identity"))
            break;
    }
    /* no identity? add it */
    if (qh==NULL) {
        if ((qh = xmalloc(sizeof(*qh)))==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            return MMP_ERR_ENOMEM;
        }
        if ((qh->id = xstrdup("identity"))==NULL) {
            free(qh);
            mmp_setError(MMP_ERR_ENOMEM);
            return MMP_ERR_ENOMEM;
        }
        qh->quality = 1.0f;
        qh->extp_list = NULL;
        if ((ret = qhead_insert(qhead, qh))!=MMP_ERR_OK) {
            xfree(qh->id);
            xfree(qh);
            return ret;
        }
    }
    /* check for '*' */
    for (p=qhead->head; p!=NULL; p=p->next) {
        qh = (t_qhead_s*)p;
        if (qh==NULL || strcmp(qh->id, "*")!=0)
            continue;
        /* ok - we've got a '*'. Drop the entry and, for each filter we've
         * got that is not present, do an insert with the '*' quality */
        rq = qh->quality;
        qhead_delete(qhead, &qh);
        for (q=filters->head; q!=NULL; q=q->next) {
            filt = (t_module_s*)q->data;
            if (filt==NULL) continue;
            qh2 = NULL;
            for (k=qhead->head; k!=NULL; k=k->next) {
                qh2 = (t_qhead_s*)k->data;
                if (qh2==NULL) continue;
                if (!strcmp(qh2->id, filt->name))
                    break;
            }
            if (qh2==NULL) {
                if ((qh2 = xmalloc(sizeof(*qh2)))==NULL) {
                    mmp_setError(MMP_ERR_ENOMEM);
                    return MMP_ERR_ENOMEM;
                }
                if ((qh2->id = xstrdup(filt->name))==NULL) {
                    free(qh2);
                    mmp_setError(MMP_ERR_ENOMEM);
                    return MMP_ERR_ENOMEM;
                }
                qh2->quality = rq;
                qh2->extp_list = NULL;
                if ((ret = qhead_insert(qhead, qh2))!=MMP_ERR_OK) {
                    xfree(qh2->id);
                    xfree(qh2);
                    return ret;
                }
            }
        }
        break;
    }
    /* drop all the filters that we can't handle */
    for (p=qhead->head; p!=NULL; p=p->next) {
        qh = (t_qhead_s*)p;
        if (qh==NULL) continue;
        if (filter_findbyid(qh->id)==NULL)
            qhead_delete(qhead, &qh);
    }
    return 0;
}

t_module_s *filter_findfilter(t_qhead_list_s *qlist)
{
    t_mmp_listelem_s *p;
    t_qhead_s *qh;
    t_module_s *mod;
    if (qlist==NULL) return NULL;
    for (p=qlist->head; p!=NULL; p=p->next) {
        qh = (t_qhead_s*)p->data;
        if (qh==NULL) continue;
        if ((mod = filter_findbyid(qh->id))!=NULL)
            return mod;
    }
    return NULL;
}

