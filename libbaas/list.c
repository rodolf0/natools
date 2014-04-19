#include <stdlib.h>
#include <stdio.h>
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

list_node_t * list_nth(const list_t *l, size_t n) {
  if (!l || n == 0 || n > l->size) return NULL;
  list_node_t *nth = l->first;
  while (--n && nth)
    nth = nth->next;
  return nth; /* assert(n == 0) */
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
  if (!l) return NULL;
  if (!l->cmp) {
    fprintf(stderr, "list find error: no comparison function\n");
    return NULL;
  }
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

list_node_t * list_insert(list_t *l, list_node_t *prev, void *d) {
  if (!l) return NULL;
  if (!prev) return list_push(l, d);
  if (!prev->next) return list_queue(l, d); /* assert prev == l->last */
  list_node_t *n = (list_node_t*)zmalloc(sizeof(list_node_t));
  if (n == NULL) return NULL;
  n->data = d;
  n->prev = prev;
  n->next = prev->next;
  /* hook node after <prev> */
  prev->next->prev = n;
  prev->next = n;
  l->size++;
  return n;
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
list_t * list_slice(const list_t *l, list_node_t *from, list_node_t *to) {
  if (!l) return NULL;
  list_t *r = list_init(NULL, l->cmp);
  if (r == NULL) return NULL;
  for (list_node_t *n = from; n && n != to; n = n->next)
    list_queue(r, n->data);
  return r;
}

list_t * list_dup(const list_t *l) {
  return list_slice(l, l->first, NULL);
}

int list_cmp(const list_t *l, const list_t *o) {
  if (!l && !o) return 0;
  if (!o) return 1;
  if (!l) return -1;
  if (l->size == 0 && o->size == 0) return 0;
  if (!l->cmp) {
    fprintf(stderr, "list cmp error: no comparison function\n");
    return 0;
  }
  list_node_t *a = l->first,
              *b = o->first;
  while (a && b) {
    int c = l->cmp(a->data, b->data);
    if (c < 0) return -1;
    if (c > 0) return 1;
    a = a->next;
    b = b->next;
  }
  if (a) return 1;
  if (b) return -1;
  return 0;
}

void list_reverse(list_t *l) {
  if (!l) return;
  list_node_t *t;
  for (list_node_t *n = l->first; n; n = t) {
    t = n->next;
    /* reverse n's links */
    n->next = n->prev;
    n->prev = t;
  }
  t = l->first;
  l->first = l->last;
  l->last = t;
  return;
}

void list_unique(list_t *l) {
  if (!l || l->size < 2) return;
  if (!l->cmp) {
    fprintf(stderr, "list unique error: no comparison function\n");
    return;
  }
  list_node_t *p = l->first;
  for (list_node_t *n = p->next; p && n; n = p->next) {
    if (l->cmp(p->data, n->data) == 0) {
      if (l->free)
        l->free(n->data);
      /* drop n: make p skip n, link to next */
      p->next = n->next;
      if (n->next)
        n->next->prev = p;
      else
        l->last = p;
      l->size--;
      free(n);
    } else {
      p = p->next;
    }
  }
}

list_t * list_concat(list_t *l1, list_t *l2) {
  if (!l1 && !l2) return NULL;
  if (!l2) return l1;
  if (!l1) return l2;
  if (l1 == l2) return list_concat(l1, list_dup(l1));
  if (l2->size == 0)  {
    free(l2);
    return l1;
  }
  if (l1->size == 0) {
    l2->cmp = l1->cmp;
    l2->free = l1->free;
    free(l1);
    return l2;
  }
  l1->last->next = l2->first;
  l2->first->prev = l1->last;
  l1->last = l2->last;
  l1->size += l2->size;
  free(l2);
  return l1;
}

list_t * list_merge(list_t *l1, list_t *l2) {
  if (!l1 && !l2) return NULL;
  if (l1 == l2) return l1;
  if (!l1) return l2;
  if (!l2) return l1;
  if (l2->size == 0)  {
    free(l2);
    return l1;
  }
  if (l1->size == 0) {
    l2->cmp = l1->cmp;
    l2->free = l1->free;
    free(l1);
    return l2;
  }
  if (!l1->cmp) {
    fprintf(stderr, "list merge error: no comparison function\n");
    return NULL;
  }
  list_t t = {.first = NULL, .last = NULL, .size = 0,
              .cmp = l1->cmp, .free = l1->free};
  while (l1->size > 0 || l2->size > 0) {
    list_t *l;
    if (l1->size == 0) l = l2;
    else if (l2->size == 0) l = l1;
    else l = (t.cmp(l1->first->data, l2->first->data) <= 0) ? l1 : l2;
    list_node_t *n = l->first;
    /* unhook node from prev list */
    l->first = n->next;
    if (l->first)
      l->first->prev = NULL;
    l->size--;
    if (l->size == 0) l->last = NULL;
    /* hook node to new list */
    n->next = NULL;
    n->prev = t.last;
    if (t.last)
      t.last->next = n;
    t.last = n;
    if (t.size == 0) t.first = t.last; /* first node */
    t.size++;
  }
  free(l2);
  *l1 = t; /* use l1's boxing */
  return l1;
}

void list_split(list_t *l, size_t na, list_t **a, list_t **b) {
  if (!l) {
    *a = *b = NULL;
    return;
  }
  if (na == 0) {
    *a = list_init(l->free, l->cmp);
    *b = l;
    return;
  }
  if (list_size(l) <= na) {
    *b = list_init(l->free, l->cmp);
    *a = l;
    return;
  }
  *a = l;
  *b = list_init(l->free, l->cmp);
  if (*b == NULL) {
    *a = *b = NULL;
    return;
  }
  list_node_t *nth = list_nth(l, na);
  list_t *bb = *b, *aa = *a;
  /* build second list */
  bb->first = nth->next;
  bb->last = l->last;
  bb->size = l->size - na;
  /* re-use l's box for first list */
  aa->first = l->first; /* noop */
  aa->last = nth;
  aa->size = na;
  /* unhook lists */
  nth->next->prev = NULL;
  nth->next = NULL;
  return;
}

list_t * list_mergesort(list_t *l) {
  if (!l) return NULL;
  if (l->size <= 1) return l;
  list_t *a, *b;
  list_split(l, l->size/2, &a, &b);
  if (a == NULL || b == NULL) return NULL;
  return list_merge(list_mergesort(a), list_mergesort(b));
}

/* vim: set sw=2 sts=2 : */
