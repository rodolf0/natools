#include <stdlib.h>
#include "baas/heap.h"

heap_t * heap_init(free_func_t hfree, cmp_func_t hcmp) {
  heap_t *h = malloc(sizeof(heap_t));
  h->bufsz = HEAP_INIT_SIZE;
  h->size = 0;
  h->data = malloc(HEAP_INIT_SIZE * sizeof(void *));
  h->free = hfree;
  h->cmp = hcmp;
  return h;
}

void heap_destroy(heap_t *h) {
  if (!h)
    return;
  if (h->free)
    while (h->size--)
      h->free(*(h->data+h->size));
  free(h->data);
  free(h);
}


/* automatic resize of the heap */
static inline void heap_resize(heap_t *h) {
  /* TODO: check realloc return value (may overwrite our cur value if fail) */
  if (h->size == h->bufsz) {
    h->bufsz *= 2;
    h->data = realloc(h->data, sizeof(void*) * h->bufsz);
  } else if (h->bufsz > HEAP_INIT_SIZE && h->size < h->bufsz/3) {
    h->bufsz = (h->bufsz/2 < HEAP_INIT_SIZE ? HEAP_INIT_SIZE : h->bufsz/2);
    h->data = realloc(h->data, sizeof(void*) * h->bufsz);
  }
}


void heap_insert(heap_t *h, void *e) {
  if (!h)
    return;
  /* let the heap grow if we're out of space */
  heap_resize(h);
  heap_bubble_up(h, e, h->size);
  h->size++;
}


void * heap_pop(heap_t *h) {
  if (!h || !h->size)
    return NULL;
  /* pop the head and sift down the last element to replace it */
  void *e = h->data[0];
  void *tmp_root = h->data[h->size-1]; // last element
  h->size--;
  heap_sift_down(h, tmp_root, 0);
  heap_resize(h);
  return e;
}


/* place element e in position idx (normaly last) and let it bubble up */
/* this overwrites the value at idx */
void heap_bubble_up(heap_t *h, void *e, size_t idx) {
  if (!h || !h->cmp) return;
  size_t piv = heap_parent(idx);
  /* while new node is bigger than parents swap them */
  while (idx > 0 && h->cmp(e, h->data[piv]) > 0) {
    h->data[idx] = h->data[piv];
    idx = piv; piv = heap_parent(idx);
  }
  h->data[idx] = e;
}


/* place e at idx and let it sift down.
 * this overwrites the value at idx */
void heap_sift_down(heap_t *h, void *e, size_t idx) {
  if (!h || !h->cmp) return;
  size_t piv = heap_greatest_child(h, idx);
  /* while not in heap order keep on sifting */
  while (piv > 0 && h->cmp(e, h->data[piv]) < 0) {
    h->data[idx] = h->data[piv]; idx = piv;
    piv = heap_greatest_child(h, idx);
  }
  h->data[idx] = e;
}


/* return the index of the greatest child (if any) or the root 0 */
size_t heap_greatest_child(heap_t *h, size_t idx) {
  if (!h || !h->cmp) return 0; /* it would never be the root */
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


/* return element's index within the heap or one off index */
size_t heap_element_idx(heap_t *h, void *e) {
  if (!h) return 0;
  size_t idx = 0;
  for (idx = 0; idx < h->size; idx++)
    if (h->data[idx] == e) break;
  return idx;
}


/* given an array of n elements of size sz make pointers to them
 * and rearange the pointers so that they form a heap.
 * hcmp and hfree will act uppon the pointers to the real elements
 * The main advantage is O(n) construction time instead of nlogn */
heap_t * heap_heapify(void *data, size_t n, size_t sz,
                      free_func_t hfree, cmp_func_t hcmp) {
  /* create the special formed heap */
  heap_t *h = malloc(sizeof(heap_t));
  h->bufsz = n; h->size = n;
  h->free = hfree; h->cmp = hcmp;
  h->data = malloc(n * sizeof(void *));
  /* set pointers to each of the elements of data */
  ssize_t i; // need sign to stop loop
  for (i = 0; i < (ssize_t)n; i++)
    h->data[i] = (data + i*sz);
  /* sift down every element starting from the last parent */
  for (i = heap_parent(n); i >= 0; i--)
    heap_sift_down(h, h->data[i], i);
  return h;
}


/* vim: set sw=2 sts=2 : */
