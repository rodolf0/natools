#include <stdlib.h>
#include "baas/heap-list.h"

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

/* vim: set sw=2 sts=2 : */
