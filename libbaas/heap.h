#ifndef _HEAP_H_
#define _HEAP_H_

#include <sys/types.h>
#include "list.h"

/* Binary heap: is a complete binary tree (all levels are complete
 *  except possibly the last one which is filled left to right).
 *  Each node must be equal or greater than each of its children
 *  according to some comparison function */

#define HEAP_INIT_SIZE  64 /* keep this a power of 2 */

typedef struct _heap {
  size_t size;   /* space actually used */
  size_t bufsz;  /* space alloc'ed */
  void **data;

  free_func_t free;
  cmp_func_t cmp;
} heap_t;

static inline size_t heap_parent(size_t idx) { return (idx-1)/2; }
static inline size_t heap_child_l(size_t idx) { return 2*idx+1; }
static inline size_t heap_child_r(size_t idx) { return 2*(idx+1); }

heap_t * heap_init(free_func_t hfree, cmp_func_t hcmp);
void heap_destroy(heap_t *h);
heap_t * heap_heapify_list(list_t *l);
heap_t * heap_heapify(void *data, size_t n, size_t sz,
                      free_func_t hfree, cmp_func_t hcmp);

void heap_bubble_up(heap_t *h, void *e, size_t idx);
void heap_insert(heap_t *h, void *e);

/* return the index of the element or next free slot */
size_t heap_element_idx(heap_t *h, void *e);

void heap_sift_down(heap_t *h, void *e, size_t idx);
void * heap_pop(heap_t *h);
size_t heap_greatest_child(heap_t *h, size_t idx);

#endif /* _HEAP_H_ */

/* vim: set sw=2 sts=2 : */
