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

/* push and pop items at the head/tail */
list_node_t * list_push(list_t *l, void *d);
list_node_t * list_queue(list_t *l, void *d);
void * list_pop(list_t *l);
void * list_dequeue(list_t *l);

/* list iteration */
size_t list_size(const list_t *l);
list_node_t * list_first(const list_t *l);
list_node_t * list_last(const list_t *l);
list_node_t * list_nth(const list_t *l, size_t n); /* 1-based */
list_node_t * list_next(const list_node_t *n);
list_node_t * list_prev(const list_node_t *n);

/* peek elements without altering the list */
void * list_data(const list_node_t *n);
void * list_peek_head(const list_t *l);
void * list_peek_tail(const list_t *l);

/* find data in list */
list_node_t * list_find(const list_t *l, void *d);
/* unlink a node from the list */
void list_remove(list_t *l, list_node_t *n);
/* insert d after <prev> node, or begining if prev is null */
list_node_t * list_insert(list_t *l, list_node_t *prev, void *d);

/* execute f with each element of the list */
void list_foreach(const list_t *l, void (*f)(void *));
/* run f on each element of l and return a result list. INFO: no cmp/free  */
list_t * list_map(const list_t *l, void * (*f)(void *));
/* copy the list. INFO: same pointers to data, no free-func */
list_t * list_dup(const list_t *l);
/* lexicographic compare of list to other */
int list_cmp(const list_t *l, const list_t *o);
/* reverse list */
void list_reverse(list_t *l);
/* drop consecutive duplicate elements (needs to be sorted) */
void list_unique(list_t *l);
/* concat lists, l1 l2 are freed, l1's free+cmp */
list_t * list_concat(list_t *l1, list_t *l2);
/* merge 2 pre-sorted lists, l1 l2 freed, l1's free+cmp */
list_t * list_merge(list_t *l1, list_t *l2);
/* divide l, put atmost na elements in a,
 * rest in b, l is freed, a b get l1's free+cmp */
void list_split(list_t *l, size_t na, list_t **a, list_t **b);
/* sort the list using mergesort algorithm, l is freed */
list_t * list_mergesort(list_t *l);

#endif /*  _LIST_H_ */

/* vim: set sw=2 sts=2 : */
