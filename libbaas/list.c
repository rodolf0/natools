#include <stdlib.h>
#include "list.h"

list_t *list_init(void (*free)(void*), int (*cmp)(const void*, const void*)) {
  list_t *l = (list_t*)malloc(sizeof(list_t));

  l->first = NULL;
  l->last = NULL;
  l->free = free;
  l->cmp = cmp;
  l->size = 0;

  return l;
}

void list_destroy(list_t *l) {
  if (!l)
    return;

  list_node_t *t, *n = l->first;
  while ((t = n)) {

    if (l->free)
      l->free(n->data);

    n = n->next;
    free(t);

    l->size--;
  }

  free(l);
}

/* find data in list */
void *list_find(list_t *l, void *d) {
  if (!l || !l->cmp)
    return NULL;

  list_node_t *node;

  for (node = l->first; node; node = node->next) {
    if (l->cmp(node->data, d) == 0)
      return node->data;
  }
  return NULL;
}

/* push item at the head of the list */
list_node_t *list_push(list_t *l, void *d) {
  if (!l)
    return NULL;

  list_node_t *n = (list_node_t*)malloc(sizeof(list_node_t));

  n->data = d;
  n->prev = NULL;
  n->next = l->first;

  if (l->first)
    l->first->prev = n;

  l->first = n;
  l->size++;
  if (l->size == 1)
    l->last = n;

  return n;
}

/* pop item from the head of the list */
void *list_pop(list_t *l) {
  if (!l)
    return NULL;

  list_node_t *n = l->first;
  void *d = NULL;

  if (n) {
    d = n->data;

    l->first = l->first->next;
    if (l->first)
      l->first->prev = NULL;

    l->size--;
    free(n);

    return d;
  } else
    return NULL;
}

/* enqueue item at the tail of the list */
list_node_t *list_queue(list_t *l, void *d) {
  if (!l)
    return NULL;

  list_node_t *n = (list_node_t*)malloc(sizeof(list_node_t));

  n->data = d;
  n->prev = l->last;
  n->next = NULL;

  if (l->last)
    l->last->next = n;

  l->last = n;
  l->size++;
  if (l->size == 1)
    l->first = n;

  return n;
}

/* dequeue item from the tail of the list */
void *list_dequeue(list_t *l) {
  if (!l)
    return NULL;

  list_node_t *n = l->last;
  void *d = NULL;

  if (n) {
    d = n->data;

    l->last = l->last->prev;
    if (l->last)
      l->last->next = NULL;

    l->size--;
    free(n);

    return d;
  } else
    return NULL;
}

/* vim: set sw=2 sts=2 : */
