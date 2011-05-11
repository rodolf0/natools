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

#if 0
/* reorders elements in heap order */
void heap_heapify(void *data, size_t n, size_t sz, cmp_func_t hcmp) {
  ssize_t i;
  /* sift down every element starting backwards */
  for (i = n-1; i>=0; i--)
    heap_sift_down(h, h->data[i], i);
  return h;
}
#endif

/* creates a heap out of a list */
heap_t * heap_heapify_list(list_t *l) {
  heap_t *h = malloc(sizeof(heap_t));
  h->bufsz = l->size;
  h->data = malloc(h->bufsz * sizeof(void *));
  h->free = NULL; /* l->free will result in double free */
  h->cmp = l->cmp;
  h->size = 0;
  list_node_t *it; ssize_t i; // must be signed so it doesn't wrap
  /* get pointers to array elements */
  for (it = l->first; it != NULL; it = it->next)
    h->data[h->size++] = it->data;
  /* sift down every element starting backwards */
  for (i = h->size-1; i>=0; i--)
    heap_sift_down(h, h->data[i], i);
  return h;
}

/* automatic resize of the heap */
static inline void heap_resize(heap_t *h) {
  if (h->size == h->bufsz) {
    /* TODO: check realloc return value (may overwrite our cur value) */
    h->data = realloc(h->data, 2*h->bufsz);
    h->bufsz *= 2;
  } else if (h->bufsz > HEAP_INIT_SIZE && h->size < h->bufsz/3) {
    h->data = realloc(h->data, h->bufsz/2);
    h->bufsz /= 2;
  }
}

/* place element e in position idx (normaly last) and let it bubble up */
/* this overwrites the value at idx */
void heap_bubble_up(heap_t *h, void *e, size_t idx) {
  size_t piv = heap_parent(idx);
  /* while new node is bigger than parents swap them */
  while (idx > 0 && h->cmp(e, h->data[piv]) > 0) {
    h->data[idx] = h->data[piv];
    idx = piv; piv = heap_parent(idx);
  }
  h->data[idx] = e;
}

void heap_insert(heap_t *h, void *e) {
  /* let the heap grow if we're out of space */
  heap_resize(h);
  heap_bubble_up(h, e, h->size);
  h->size++;
}

void * heap_pop(heap_t *h) {
  if (!h->size)
    return NULL;
  /* pop the head and sift down the last element to replace it */
  void *e = h->data[0];
  void *tmp_root = h->data[h->size-1]; // last element
  h->size--;
  heap_sift_down(h, tmp_root, 0);
  heap_resize(h);
  return e;
}

/* return element's index within the heap or one off index */
size_t heap_element_idx(heap_t *h, void *e) {
  size_t idx = 0;
  for (idx = 0; idx < h->size; idx++)
    if (h->data[idx] == e) break;
  return idx;
}

/* place e at idx and let it sift down.
 * this overwrites the value at idx */
void heap_sift_down(heap_t *h, void *e, size_t idx) {
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

/* vim: set sw=2 sts=2 : */
