#include <stdlib.h>
#include "list.h"

list_t * list_init(free_func_t free, cmp_func_t cmp) {
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

/* concatenate the 2nd list to the 1st one, l2 will be destroyed */
void list_concat(list_t *l1, list_t *l2) {
  if (!l1 || !l2)
    return;
  /* link the 2 lists */
  if (l2->first)
    l2->first->prev = l1->last;
  if (l1->last)
    l1->last->next = l2->first;
  /* update l1's information */
  l1->last = l2->last;
  l1->size += l2->size;
  /* this means l1 was empty */
  if (l1->size == l2->size)
    l1->first = l2->first;
  /* free the list structure without releasing it's nodes */
  free(l2);
}

/* merge 2 lists into a new list, old lists will be destroyed */
list_t * list_merge(list_t *l1, list_t *l2) {
  if (!l1 || !l2 || !l1->cmp)
    return NULL;
  /* result list */
  list_t *r = list_init(l1->free, l1->cmp);
  /* merge lists */
  while (l1->size && l2->size) {
    if (l1->cmp(l1->first->data, l2->first->data) <= 0)
      list_queue(r, list_pop(l1));
    else
      list_queue(r, list_pop(l2));
  }
  /* append remaining elements */
  while (l1->size) list_queue(r, list_pop(l1));
  while (l2->size) list_queue(r, list_pop(l2));
  /* free empty lists */
  list_destroy(l1);
  list_destroy(l2);
  return r;
}

/* split l in half returning left and right lists */
void list_split_half(list_t *l, list_t **r) {
  if (!l) {
    *r = NULL;
    return;
  }
  /* search for the middle node */
  size_t split = l->size / 2;
  list_node_t *r_first = l->first;
  while (split--) r_first = r_first->next;
  /* split l into 2 lists of half the size */
  *r = list_init(l->free, l->cmp);
  /* adjust lists l and l2 */
  (*r)->last = l->last;
  (*r)->first = r_first;
  l->last = r_first->prev;
  (*r)->size = l->size - l->size / 2; // watch integer math
  l->size /= 2;
  /* unlink nodes on the cutting point */
  if (r_first->prev)
    r_first->prev->next = NULL;
  r_first->prev = NULL;
}

/* sort the list using mergesort algorithm */
list_t * list_mergesort(list_t *l) {
  if (!l)
    return NULL;
  if (l->size <= 1)
    return l;
  else {
    list_t *r = NULL;
    list_split_half(l, &r);
    return list_merge(list_mergesort(l), list_mergesort(r));
  }
}

/* find data in list */
void * list_find(list_t *l, void *d) {
  if (!l || !l->cmp)
    return NULL;

  list_node_t *node;
  for (node = l->first; node; node = node->next) {
    if (l->cmp(node->data, d) == 0)
      return node->data;
  }
  return NULL;
}

/* push item at the head of the list, return the created node */
list_node_t * list_push(list_t *l, void *d) {
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
void * list_pop(list_t *l) {
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

/* peek elements without removing them */
void * list_peek_head(list_t *l) {
  if (l && l->first)
    return l->first->data;
  else
    return NULL;
}
void * list_peek_tail(list_t *l) {
  if (l && l->last)
    return l->last->data;
  else
    return NULL;
}

/* enqueue item at the tail of the list */
list_node_t * list_queue(list_t *l, void *d) {
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
void * list_dequeue(list_t *l) {
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
