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

static t_qhead_s *qhead_create_and_add(t_qhead_list_s *list, const char *id,
                                                                    float q)
{
    t_qhead_s *ret;
    if ((ret = qhead_create(id))==NULL) {
        mmp_setError(MMP_ERR_GENERIC);
        return NULL;
    }
    ret->quality = q;
    ret->extp_list = NULL;
    if (qhead_list_insert(list, ret)!=MMP_ERR_OK) {
        qhead_destroy(&ret);
        return NULL;
    }
    return ret;
}

/* for filter queues we have a few rules:
 * - identity filter (if not present) has to be considered present. We choose
 *   quality = 1.000
 * - if a '*' is present, it marks all the available filters (identity too)
 *
 * TODO This sanitize function is a cpu-cycle eater! Too much cycles!
 * TODO check for the case of empty list at the end of the function: maybe we
 *      deal with it later, but I'm not sure. */
ret_t filter_sanitize_queue(t_qhead_list_s **qhead)
{
    t_mmp_listelem_s *p, *q, *k;
    t_module_list_s *filters;
    t_module_s *filt;
    float rq;
    t_qhead_s *qh, *qh2;
    filters = module_getfilterlist();
    /* check for identity */
    qh = NULL;
    if (*qhead==NULL) {
        *qhead = mmp_list_create();
    }
    for (p=(*qhead)->head; p!=NULL; p=p->next) {
        qh = (t_qhead_s*)p;
        if (qh==NULL) continue;
        if (!strcmp(qh->id, "identity"))
            break;
        qh = NULL;
    }
    /* no identity? add it */
    if (qh==NULL) {
        if ((qh = qhead_create_and_add(*qhead, "identity", 1.0f))==NULL) {
            mmp_setError(MMP_ERR_GENERIC);
            return MMP_ERR_GENERIC;
        }
    }
    /* check for '*' */
    for (p=(*qhead)->head; p!=NULL; p=p->next) {
        qh = (t_qhead_s*)p;
        if (qh==NULL || strcmp(qh->id, "*")!=0)
            continue;
        /* ok - we've got a '*'. Drop the entry and, for each filter we've
         * got that is not present, do an insert with the '*' quality */
        rq = qh->quality;
        qhead_list_delete(*qhead, &qh);
        for (q=filters->head; q!=NULL; q=q->next) {
            filt = (t_module_s*)q->data;
            if (filt==NULL) continue;
            qh2 = NULL;
            for (k=(*qhead)->head; k!=NULL; k=k->next) {
                qh2 = (t_qhead_s*)k->data;
                if (qh2==NULL) continue;
                if (!strcmp(qh2->id, filt->name))
                    break;
                qh2 = NULL;
            }
            if (qh2==NULL) {
                if ((qh2 = qhead_create_and_add(*qhead, filt->name, rq))==NULL) {
                    mmp_setError(MMP_ERR_GENERIC);
                    return MMP_ERR_GENERIC;
                }
            }
        }
        break;
    }
    /* drop all the filters that we can't handle */
    for (p=(*qhead)->head; p!=NULL; p=q) {
        q = p->next;
        qh = (t_qhead_s*)p->data;
        if (qh==NULL) continue;
        if (filter_findbyid(qh->id)==NULL) {
            printf("Delete %s\n", qh->id);
            qhead_list_delete(*qhead, &p);
        }
    }
    return MMP_ERR_OK;
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

