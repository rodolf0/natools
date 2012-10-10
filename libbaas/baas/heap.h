#ifndef _HEAP_H_
#define _HEAP_H_

#include "common.h"

/* Binary heap: is a complete binary tree (all levels are complete
 *  except possibly the last one which is filled left to right).
 *  Each node must be equal or greater than each of its children
 *  according to some comparison function */
/* http://en.wikipedia.org/wiki/Binary_heap */

typedef struct _heap {
  size_t size;   /* space actually used */
  size_t bufsz;  /* space alloc'ed */
  void **data;

  free_func_t free;
  cmp_func_t cmp;
} heap_t;

/* basic lookup functions */
static inline size_t heap_parent(size_t idx) { return (idx ? (idx-1)/2 : 0); }
static inline size_t heap_child_l(size_t idx) { return 2*idx+1; }
static inline size_t heap_child_r(size_t idx) { return 2*(idx+1); }
/* get the index of the greatest child of node at idx */
ssize_t heap_greatest_child(heap_t *h, size_t idx);

/* constructor / destructor */
heap_t * heap_init(free_func_t hfree, cmp_func_t hcmp);
void heap_destroy(heap_t *h);

/* heap operation */
ssize_t heap_insert(heap_t *h, void *e);
void * heap_pop(heap_t *h);
ssize_t heap_bubble_up(heap_t *h, void *e, size_t idx);
ssize_t heap_sift_down(heap_t *h, void *e, size_t idx);

/* return the index of the element or next free slot */
ssize_t heap_find(heap_t *h, void *e);

/* given an array of n elements of size sz make pointers to them
 * and rearange the pointers so that they form a heap.
 * hcmp and hfree will act uppon the pointers to the real elements
 * The main advantage is O(n) construction time instead of nlogn */
heap_t * heap_heapify(void *data, size_t n, size_t sz,
                      free_func_t hfree, cmp_func_t hcmp);

#endif /* _HEAP_H_ */

/* vim: set sw=2 sts=2 : */
