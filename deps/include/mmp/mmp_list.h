#ifndef H_MMP_LIST_H
#define H_MMP_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include "mmp_memory.h"
#include "mmp_trace.h"

struct list_s {
    int nelems;
    struct listelem_s *head, *tail;
};

struct listelem_s {
    void *data;
    struct listelem_s *next, *prev;
};

/* creates an empty list */
struct list_s *list_create(void);
/* delete a complete list */
void list_delete(struct list_s **list);
/* delete a complete list calling a function on each data before deletion */
void list_delete_withdata(struct list_s **list, void(*cback)(void**));
/* add data to the list */
ret_t list_add_data(struct list_s *list, void *data);
/* add sorted data to the list */
ret_t list_add_data_sorted(struct list_s *list, void *data,
                                                    int(*comp)(void*, void*));
/* delete an element from the list, return the element data */
void *list_del_elem(struct list_s *list, struct listelem_s **elem);
/* delete an element from the list, by data */
void* list_del_elem_by_data(struct list_s *list, void *data);
/* find data in the list */
struct listelem_s *list_find_data(struct list_s *list, void *data);
/* find data in the list by comparer lambda */
struct listelem_s *list_find_data_lambda(struct list_s *list, void *data,
                                                    int (*comp)(void*, void*));
/* execute a lambda function for each element in list */
void list_lambda_elem(struct list_s *list, void(*fnc)(struct listelem_s *));
/* execute a lambda function for each element data in list */
void list_lambda_data(struct list_s *list, void(*fnc)(void*));
/* execute a lambda function for each element data in list, with extra params*/
void list_lambda_data_ext(struct list_s *list, void(*fnc)(void*, void*),
                                                                void *params);
/* execute a lambda function for each element data in list, with extra params*/
void list_lambda_data_ext_rev(struct list_s *list, void(*fnc)(void*, void*),
                                                                void *params);
/* swap two list elements */
void list_swap_elems(struct listelem_s *e1, struct listelem_s *e2);
/* sory a list by elements, with a comparer lambda */
void list_sort_by_data(struct list_s *list, int (*comp)(void*, void*));

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_list_unittest(struct mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_LIST_H */
