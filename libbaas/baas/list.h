#ifndef _LIST_H_
#define _LIST_H_

#include "common.h"

typedef struct list_node_t list_node_t;
typedef struct list_t list_t;

/* constructor / destructor */
list_t * list_init(free_func_t, cmp_func_t);
void list_destroy(list_t *l);

/* set free/cmp functions. return previous version */
free_func_t list_set_free(list_t *l, free_func_t);
cmp_func_t list_set_cmp(list_t *l, cmp_func_t);

list_node_t *list_first(const list_t *l);
list_node_t *list_last(const list_t *l);
size_t list_size(const list_t *l);

list_node_t *list_node_next(const list_node_t *n);
list_node_t *list_node_prev(const list_node_t *n);
void *list_node_data(const list_node_t *n);

/* push and pop items at the head of the list */
list_node_t * list_push(list_t *l, void *d);
void * list_pop(list_t *l);

/* queue and dequeue items at the tail of the list */
list_node_t * list_queue(list_t *l, void *d);
void * list_dequeue(list_t *l);

/* peek elements without removing them */
void * list_peek_head(list_t *l);
void * list_peek_tail(list_t *l);

/* unlink a node from the list */
void list_remove(list_t *l, list_node_t *n);

/* execute f with each element of the list */
void list_foreach(list_t *l, void (*f)(void *));
/* run f on each element of l and return a result list. INFO: no cmp/free  */
list_t * list_map(list_t *l, void * (*f)(void *));

/* find data in list */
list_node_t * list_find(list_t *l, void *d);

/* return the concat of both lists, l1 and l2 will be destroyed */
list_t * list_concat(list_t *l1, list_t *l2);
/* return a copy of the list. INFO: data elements will point to the same */
list_t * list_dup(list_t *l);
/* merge 2 lists into a new list, old lists will be destroyed */
list_t * list_merge(list_t *l1, list_t *l2);
/* split l in half returning left and right lists, l is destroyed */
void list_split_half(list_t *l, list_t **a, list_t **b);
/* sort the list using mergesort algorithm, original will be destroyed */
list_t * list_mergesort(list_t *l);

#endif /*  _LIST_H_ */

/* vim: set sw=2 sts=2 : */
