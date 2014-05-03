#ifndef _HEAP_H_
#define _HEAP_H_

#include "common.h"

typedef struct heap_t heap_t;

/* constructor / destructor */
heap_t * heap_init(free_func_t hfree, cmp_func_t hcmp);
void heap_destroy(heap_t *h);

/* set free/cmp functions. return previous version */
free_func_t heap_set_free(heap_t *h, free_func_t);
cmp_func_t heap_set_cmp(heap_t *h, cmp_func_t);

/* get heap size/capacity */
size_t heap_size(const heap_t *h);
size_t heap_capacity(const heap_t *h);

/* basic lookup functions */
static inline size_t heap_parent(size_t idx) { return (idx ? (idx-1)/2 : 0); }
static inline size_t heap_child_l(size_t idx) { return 2*idx+1; }
static inline size_t heap_child_r(size_t idx) { return 2*(idx+1); }

/* get the index of the greatest child of node at idx */
ssize_t heap_greatest_child(heap_t *h, size_t idx);

/* heap operation */
ssize_t heap_insert(heap_t *h, void *e);
void * heap_pop(heap_t *h);
void * heap_get(const heap_t *h, size_t idx);

/* insert element at idx and put in heap order */
ssize_t heap_bubble_up(heap_t *h, void *e, size_t idx);
ssize_t heap_sift_down(heap_t *h, void *e, size_t idx);

/* return the index of the element or -1 */
ssize_t heap_find(heap_t *h, void *e);

/* given an array of n elements of size sz make pointers to them
 * and rearange the pointers so that they form a heap.
 * hcmp and hfree will act uppon the pointers to the real elements
 * The main advantage is O(n) construction time instead of nlogn */
heap_t * heap_heapify(void *data, size_t n, size_t sz,
                      free_func_t hfree, cmp_func_t hcmp);

#endif /* _HEAP_H_ */

/* vim: set sw=2 sts=2 : */
