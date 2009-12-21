#ifndef _LIST_H_
#define _LIST_H_

#include <sys/types.h> /* size_t */

typedef struct _list_node {
  struct _list_node *prev;
  struct _list_node *next;
  void *data;
} list_node_t;

typedef struct _list {
  list_node_t *first;
  list_node_t *last;

  size_t size;
  void (*free)(void*);
} list_t;

list_t *list_init(void (*free)(void*));
void list_destroy(list_t *l);

/* push item at the head of the list */
list_node_t *list_push(list_t *l, void *d);

/* pop item from the head of the list */
void *list_pop(list_t *l);

/* enqueue item at the tail of the list */
list_node_t *list_queue(list_t *l, void *d);

/* dequeue item from the tail of the list */
void *list_dequeue(list_t *l);

#endif /*  _LIST_H_ */

/* vim: set sw=2 sts=2 : */
