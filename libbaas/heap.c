#include <stdio.h>
#include <stdlib.h>
#include "baas/heap.h"

struct heap_t {
  size_t size;   /* space actually used */
  size_t cap;  /* space alloc'ed */
  void **data;

  free_func_t free;
  cmp_func_t cmp;
};

#define HEAP_INIT_SIZE  8 /* keep this a power of 2 */

/*******************************************************/
heap_t * heap_init(free_func_t hfree, cmp_func_t hcmp) {
  if (!hcmp) {
    fprintf(stderr, "heap error: no comparison function\n");
    return NULL;
  }
  heap_t *h = (heap_t*)zmalloc(sizeof(heap_t));
  h->cap = HEAP_INIT_SIZE;
  h->size = 0;
  h->data = (void**)zmalloc(HEAP_INIT_SIZE * sizeof(void *));
  h->free = hfree;
  h->cmp = hcmp;
  return h;
}

void heap_destroy(heap_t *h) {
  if (!h) return;
  if (h->free) {
    while (h->size--)
      h->free(h->data[h->size]);
  }
  free(h->data);
  free(h);
}

/*******************************************************/
free_func_t heap_set_free(heap_t *h, free_func_t f) {
  free_func_t old = h->free;
  h->free = f;
  return old;
}

cmp_func_t heap_set_cmp(heap_t *h, cmp_func_t c) {
  cmp_func_t old = h->cmp;
  h->cmp = c;
  return old;
}

/*******************************************************/
size_t heap_size(const heap_t *h) {
  return h ? h->size : 0;
}

size_t heap_capacity(const heap_t *h) {
  return h ? h->cap : 0;
}

/*******************************************************/
// TODO: adjust return for realloc failure
static void heap_resize(heap_t *h) {
  if (h->size >= h->cap) {
    h->cap *= 2;
  } else if (h->cap >= 2 * HEAP_INIT_SIZE && h->size < h->cap/3) {
    h->cap /= 2;
  }
  h->data = (void**)xrealloc(h->data, sizeof(void*) * h->cap);
}


/*******************************************************/
ssize_t heap_insert(heap_t *h, void *e) {
  if (!h) return -1;
  heap_resize(h);
  size_t idx = heap_bubble_up(h, e, h->size);
  h->size++;
  return idx;
}

void * heap_pop(heap_t *h) {
  if (!h || h->size == 0) return NULL;
  void *e = h->data[0];
  void *tmp_root = h->data[h->size-1];
  h->size--;
  heap_sift_down(h, tmp_root, 0);
  heap_resize(h);
  return e;
}

void * heap_get(const heap_t *h, size_t idx) {
  if (!h || h->size < idx) return NULL;
  return h->data[idx];
}

/*******************************************************/
/* place element e in position idx (normaly last) and let it bubble up */
/* this overwrites the value at idx */
ssize_t heap_bubble_up(heap_t *h, void *e, size_t idx) {
  if (!h) return -1;
  size_t piv = heap_parent(idx);
  /* while new node is bigger than parents swap them */
  while (idx > 0 && h->cmp(e, h->data[piv]) > 0) {
    h->data[idx] = h->data[piv];
    idx = piv; piv = heap_parent(idx);
  }
  h->data[idx] = e;
  return idx;
}

/* place e at idx and let it sift down.
 * this overwrites the value at idx */
ssize_t heap_sift_down(heap_t *h, void *e, size_t idx) {
  if (!h) return -1;
  if (!h->cmp) {
    fprintf(stderr, "heap error: no comparison function\n");
    return -1;
  }
  size_t piv = heap_greatest_child(h, idx);
  /* while not in heap order keep on sifting */
  while (piv > 0 && h->cmp(e, h->data[piv]) < 0) {
    h->data[idx] = h->data[piv]; idx = piv;
    piv = heap_greatest_child(h, idx);
  }
  h->data[idx] = e;
  return idx;
}

/* return the index of the greatest child (if any) or the root 0 */
ssize_t heap_greatest_child(heap_t *h, size_t idx) {
  if (!h) return -1;
  if (!h->cmp) {
    fprintf(stderr, "heap error: no comparison function\n");
    return -1;
  }
  size_t left = heap_child_l(idx);
  size_t right = heap_child_r(idx);
  /* since we use a complete binary tree, if right => left too */
  if (right < h->size)
    return (h->cmp(h->data[left], h->data[right]) > 0 ? left : right);
  else if (left < h->size)
    return left;
  else
    return 0;
}

/*******************************************************/
/* return element's index within the heap or one off index */
ssize_t heap_find(heap_t *h, void *e) {
  if (!h) return -1;
  if (!h->cmp) {
    fprintf(stderr, "heap error: no comparison function\n");
    return -1;
  }
  for (size_t idx = 0; idx < h->size; ++idx) {
    if (h->cmp(h->data[idx], e) == 0)
      return idx;
  }
  return -1;
}

/* given an array of n elements of size sz make pointers to them
 * and rearange the pointers so that they form a heap.
 * hcmp and hfree will act uppon the pointers to the real elements
 * The main advantage is O(n) construction time instead of nlogn */
heap_t * heap_heapify(void *data, size_t n, size_t sz,
                      free_func_t hfree, cmp_func_t hcmp) {
  /* create the special formed heap */
  heap_t *h = (heap_t*)malloc(sizeof(heap_t));
  h->cap = n; h->size = n;
  h->free = hfree; h->cmp = hcmp;
  h->data = (void**)malloc(n * sizeof(void *));
  /* set pointers to each of the elements of data */
  ssize_t i; // need sign to stop loop
  for (i = 0; i < (ssize_t)n; i++)
    h->data[i] = ((char*)data + i*sz);
  /* sift down every element starting from the last parent */
  for (i = heap_parent(n); i >= 0; i--)
    heap_sift_down(h, h->data[i], i);
  return h;
}

/* vim: set sw=2 sts=2 : */
