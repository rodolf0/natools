#include <stdlib.h>
#include "baas/list.h"
#include "baas/common.h"

struct list_node_t {
  struct list_node_t *prev;
  struct list_node_t *next;
  void *data;
};

struct list_t {
  list_node_t *first;
  list_node_t *last;

  size_t size;
  free_func_t free;
  cmp_func_t cmp;
};

/*******************************************************/
list_t * list_init(free_func_t lfree, cmp_func_t cmp) {
  list_t *l = (list_t*)zmalloc(sizeof(list_t));
  l->free = lfree;
  l->cmp = cmp;
  l->size = 0;
  return l;
}

void list_destroy(list_t *l) {
  if (!l) return;
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

/*******************************************************/
free_func_t list_set_free(list_t *l, free_func_t f) {
  free_func_t old = l->free;
  l->free = f;
  return old;
}

cmp_func_t list_set_cmp(list_t *l, cmp_func_t c) {
  cmp_func_t old = l->cmp;
  l->cmp = c;
  return old;
}

/*******************************************************/
list_node_t * list_push(list_t *l, void *d) {
  if (!l) return NULL;
  list_node_t *n = (list_node_t*)zmalloc(sizeof(list_node_t));
  if (n == NULL) return NULL;
  n->data = d;
  n->next = l->first;
  /* hook node at the head */
  if (l->first)
    l->first->prev = n;
  l->first = n;
  /* hook node at the tail if only one */
  if (l->size == 0) l->last = n;
  l->size++;
  return n;
}

void * list_pop(list_t *l) {
  if (!l || !l->first) return NULL;
  void *d = l->first->data;
  list_node_t *n = l->first;
  /* attach head to next node */
  l->first = l->first->next;
  if (l->first)
    l->first->prev = NULL;
  l->size--;
  /* if no elements left, tail was pointing here too */
  if (l->size == 0) l->last= NULL;
  free(n);
  return d;
}

list_node_t * list_queue(list_t *l, void *d) {
  if (!l) return NULL;
  list_node_t *n = (list_node_t*)zmalloc(sizeof(list_node_t));
  if (n == NULL) return NULL;
  n->data = d;
  n->prev = l->last;
  /* hook node at the tail */
  if (l->last)
    l->last->next = n;
  l->last = n;
  /* if only one element, it's both first and last */
  if (l->size == 0) l->first = n;
  l->size++;
  return n;
}

void * list_dequeue(list_t *l) {
  if (!l || !l->last) return NULL;
  void *d = l->last->data;
  list_node_t *n = l->last;
  /* attach tail to previous node */
  l->last = l->last->prev;
  if (l->last)
    l->last->next = NULL;
  l->size--;
  /* if no elements left, head was pointing here too */
  if (l->size == 0) l->first = NULL;
  free(n);
  return d;
}

/*******************************************************/
size_t list_size(const list_t *l) {
  return l ? l->size : 0;
}

list_node_t *list_first(const list_t *l) {
  return l ? l->first : NULL;
}

list_node_t *list_last(const list_t *l) {
  return l ? l->last : NULL;
}

list_node_t *list_next(const list_node_t *n) {
  return n ? n->next : NULL;
}

list_node_t *list_prev(const list_node_t *n) {
  return n ? n->prev : NULL;
}

/*******************************************************/
void *list_data(const list_node_t *n) {
  return n ? n->data : NULL;
}

void * list_peek_head(const list_t *l) {
  return (l && l->first) ? l->first->data : NULL;
}

void * list_peek_tail(const list_t *l) {
  return (l && l->last) ? l->last->data : NULL;
}

/*******************************************************/
list_node_t * list_find(const list_t *l, void *d) {
  if (!l || !l->cmp) return NULL;
  for (list_node_t *node = l->first; node; node = node->next) {
    if (l->cmp(node->data, d) == 0)
      return node;
  }
  return NULL;
}

void list_remove(list_t *l, list_node_t *n) {
  if (!l || l->size == 0) return;
  /* thread surroundings */
  if (n->next)
    n->next->prev = n->prev;
  if (n->prev)
    n->prev->next = n->next;
  /* adjust ends */
  if (l->first == n)
    l->first = n->next;
  if (l->last == n)
    l->last = n->prev;
  l->size--;
  if (l->free)
    l->free(n->data);
  free(n);
}

/*******************************************************/
void list_foreach(const list_t *l, void (*f)(void *)) {
  if (!l || !f) return;
  for (list_node_t *node = l->first; node; node = node->next)
    (*f)(node->data);
}

list_t * list_map(const list_t *l, void * (*f)(void *)) {
  if (!l || !f) return NULL;
  list_t *r = list_init(NULL, NULL);
  if (r == NULL) return NULL;
  for (list_node_t *node = l->first; node; node = node->next)
    list_queue(r, (*f)(node->data));
  return r;
}


/*******************************************************/
list_t * list_concat(list_t *l1, list_t *l2) {
  if (!l1 && !l2) return NULL;
  else if (!l1) return l2;
  else if (!l2) return l1;
  list_t *r = list_init(l1->free, l1->cmp);
  if (r == NULL) return NULL;
  /* glue lists together */
  r->first = (l1->first ? l1->first : l2->first);
  r->last = (l2->last ? l2->last : l1->last);
  r->size = l1->size + l2->size;
  if (l2->first)
    l2->first->prev = l1->last;
  if (l1->last)
    l1->last->next = l2->first;
  /* free the list structure without releasing it's nodes */
  free(l1);
  free(l2);
  return r;
}

list_t * list_dup(const list_t *l) {
  if (!l) return NULL;
  list_t *r = list_init(l->free, l->cmp);
  if (r == NULL) return NULL;
  for (list_node_t *node = l->first; node; node = node->next)
    list_queue(r, node->data);
  return r;
}

list_t * list_merge(list_t *l1, list_t *l2) {
  if (!l1 && !l2) return NULL;
  else if (!l1) return l2;
  else if (!l2) return l1;
  if (!l1->cmp) return NULL;
  list_t *r = list_init(l1->free, l1->cmp);
  if (r == NULL) return NULL;
  /* merge lists; they must be sorted */
  while (l1->size && l2->size) {
    if (l1->cmp(l1->first->data, l2->first->data) <= 0)
      list_queue(r, list_pop(l1));
    else
      list_queue(r, list_pop(l2));
  }
  /* append remaining elements */
  while (l1->size) list_queue(r, list_pop(l1));
  while (l2->size) list_queue(r, list_pop(l2));
  /* free original lists metadata */
  free(l1);
  free(l2);
  return r;
}

void list_split(list_t *l, list_t **a, list_t **b) {
  if (!l) {
    *a = *b = NULL;
    return;
  }
  /* initialize the lists */
  *a = list_init(l->free, l->cmp);
  *b = list_init(l->free, l->cmp);
  if (*a == NULL || *b == NULL) {
    list_destroy(*a);
    list_destroy(*b);
    *a = *b = NULL;
    return;
  }
  /* search for the middle node */
  size_t half = (l->size+1) / 2;
  list_node_t *b_first;
  for (b_first = l->first;
       b_first && half;
       half--, b_first = b_first->next);
  /* adjust lists a and b */
  if (l->size > 0) {
    (*a)->first = l->first;
    (*a)->last = (l->size > 1 ? b_first->prev : l->first);
    (*a)->size = (l->size+1)/2;
    /* second list only has elements if more than 1 in original list */
    if (l->size > 1) {
      (*b)->first = b_first;
      (*b)->last = l->last;
      (*b)->size = l->size - (*a)->size;
    }
  }
  /* unlink cutting node */
  if (b_first) {
    if (b_first->prev)
      b_first->prev->next = NULL;
    b_first->prev = NULL;
  }
  free(l);
}

list_t * list_mergesort(list_t *l) {
  if (!l) return NULL;
  if (l->size <= 1) return l;
  list_t *a, *b;
  list_split(l, &a, &b);
  if (a == NULL || b == NULL) return NULL;
  return list_merge(list_mergesort(a), list_mergesort(b));
}

/* vim: set sw=2 sts=2 : */
