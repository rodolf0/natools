#include <stdlib.h>
#include "baas/list.h"

list_t * list_init(free_func_t lfree, cmp_func_t cmp) {
  list_t *l = (list_t*)malloc(sizeof(list_t));
  l->first = NULL;
  l->last = NULL;
  l->free = lfree;
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


/* push item at the head of the list, return the created node */
list_node_t * list_push(list_t *l, void *d) {
  if (!l)
    return NULL;

  list_node_t *n = (list_node_t*)malloc(sizeof(list_node_t));
  n->data = d; n->prev = NULL; n->next = l->first;

  /* lock for concurrent access */
  if (l->first)
    l->first->prev = n;
  l->first = n;
  /* if this is the only element, then it's both first and last */
  if (l->size == 0) l->last = n;
  l->size++;
  /* unlock */

  return n;
}


/* pop item from the head of the list */
void * list_pop(list_t *l) {
  if (!l || !l->first)
    return NULL;

  void *d = l->first->data;
  list_node_t *n = l->first;
  /* lock for concurrent access */
  l->first = l->first->next;
  if (l->first)
    l->first->prev = NULL;
  l->size--;
  /* if no elements left, then last pointer was pointing here too */
  if (l->size == 0) l->last= NULL;
  /* unlock */

  free(n);

  return d;
}


/* enqueue item at the tail of the list */
list_node_t * list_queue(list_t *l, void *d) {
  if (!l)
    return NULL;

  list_node_t *n = (list_node_t*)malloc(sizeof(list_node_t));
  n->data = d; n->prev = l->last; n->next = NULL;

  /* lock for concurrent access */
  if (l->last)
    l->last->next = n;
  l->last = n;
  /* if this is the only element, then it's both first and last */
  if (l->size == 0) l->first = n;
  l->size++;
  /* unlock */

  return n;
}


/* dequeue item from the tail of the list */
void * list_dequeue(list_t *l) {
  if (!l || !l->last)
    return NULL;

  void *d = l->last->data;
  list_node_t *n = l->last;
  /* lock for concurrent access */
  l->last = l->last->prev;
  if (l->last)
    l->last->next = NULL;
  l->size--;
  /* if no elements left, then first pointer was pointing here too */
  if (l->size == 0) l->first = NULL;
  /* unlock */

  free(n);

  return d;
}


/* peek elements without removing them */
void * list_peek_head(list_t *l) {
  return (l && l->first) ? l->first->data : NULL;
}

void * list_peek_tail(list_t *l) {
  return (l && l->last) ? l->last->data : NULL;
}


/* execute f with each element of the list */
void list_foreach(list_t *l, void (*f)(void *)) {
  if (!l || !f)
    return;

  list_node_t *node;
  for (node = l->first; node; node = node->next)
    (*f)(node->data);
}

/* run f on each element of l and return a result list. INFO: no cmp/free  */
list_t * list_map(list_t *l, void * (*f)(void *)) {
  if (!l || !f)
    return NULL;

  list_t *r = list_init(NULL, NULL);
  list_node_t *node;
  for (node = l->first; node; node = node->next)
    list_queue(r, (*f)(node->data));

  return r;
}

/* find data in list */
list_node_t * list_find(list_t *l, void *d) {
  if (!l || !l->cmp)
    return NULL;

  list_node_t *node;
  for (node = l->first; node; node = node->next) {
    if (l->cmp(node->data, d) == 0)
      return node;
  }
  return NULL;
}


/* return the concat of both lists, l1 and l2 will be destroyed */
list_t * list_concat(list_t *l1, list_t *l2) {
  if (!l1 || !l2)
    return NULL;
  /* result list */
  list_t *r = list_init(l1->free, l1->cmp);
  /* glue lists together */
  r->first = (l1->first ? l1->first : l2->first);
  r->last = (l2->last ? l2->last : l1->last);
  r->size = l1->size + l2->size;
  if (l2->first) l2->first->prev = l1->last;
  if (l1->last) l1->last->next = l2->first;
  /* free the list structure without releasing it's nodes */
  free(l1); free(l2);
  return r;
}


/* return a copy of the list. INFO: data elements will point to the same */
list_t * list_dup(list_t *l) {
  if (!l)
    return NULL;
  list_t *r = list_init(l->free, l->cmp);
  list_node_t *node;
  for (node = l->first; node; node = node->next)
    list_queue(r, node->data);
  return r;
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
  free(l1); free(l2);
  return r;
}

/* split l in half returning left and right lists, l is destroyed */
void list_split_half(list_t *l, list_t **a, list_t **b) {
  if (!l) {
    *a = NULL; *b = NULL;
    return;
  }
  /* initialize the lists */
  *a = list_init(l->free, l->cmp);
  *b = list_init(l->free, l->cmp);
  /* search for the middle node */
  size_t half = (l->size+1) / 2; list_node_t *b_first;
  for (b_first = l->first; b_first && half; half--, b_first = b_first->next);
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
    if (b_first->prev) b_first->prev->next = NULL;
    b_first->prev = NULL;
  }
  /* cleanup*/
  free(l);
}

/* sort the list using mergesort algorithm */
list_t * list_mergesort(list_t *l) {
  if (!l)
    return NULL;
  if (l->size <= 1)
    return l;
  list_t *a, *b;
  list_split_half(l, &a, &b);
  return list_merge(list_mergesort(a), list_mergesort(b));
}

/* vim: set sw=2 sts=2 : */
