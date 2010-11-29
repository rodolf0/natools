#ifndef _LIST_H_
#define _LIST_H_

#include <sys/types.h> /* size_t */

typedef int (*cmp_func_t)(const void *, const void *);
typedef void (*free_func_t)(void *);

typedef struct _list_node {
  struct _list_node *prev;
  struct _list_node *next;
  void *data;
} list_node_t;

typedef struct _list {
  list_node_t *first;
  list_node_t *last;

  size_t size;
  free_func_t free;
  cmp_func_t cmp;
} list_t;

list_t * list_init(free_func_t, cmp_func_t);
void list_destroy(list_t *l);

/* concatenate the 2nd list to the 1st one, l2 will be destroyed */
void list_concat(list_t *l1, list_t *l2);

/* merge 2 lists into a new list, old lists will be destroyed */
list_t * list_merge(list_t *l1, list_t *l2);

/* split l in half returning left and right lists */
void list_split_half(list_t *l, list_t **r);

/* sort the list using mergesort algorithm, original will be destroyed */
list_t * list_mergesort(list_t *l);

/* push item at the head of the list */
list_node_t * list_push(list_t *l, void *d);

/* pop item from the head of the list */
void * list_pop(list_t *l);

/* peek elements without removing them */
void * list_peek_head(list_t *l);
void * list_peek_tail(list_t *l);

/* enqueue item at the tail of the list */
list_node_t * list_queue(list_t *l, void *d);

/* dequeue item from the tail of the list */
void * list_dequeue(list_t *l);

/* find data in list */
void * list_find(list_t *l, void *d);

#endif /*  _LIST_H_ */

/* vim: set sw=2 sts=2 : */
