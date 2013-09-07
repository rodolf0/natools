#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include "baas/sort.h"

/* mergesort: sorts n elements of size s using cmp function to compare them.
 *  Elements must be adjacent in memory */

void * mergesort(const void *a, size_t n, size_t sz, cmp_func_t cmp) {
  if (n == 1)
    return merge(a, 1, NULL, 0, sz, cmp); // let merge malloc

  void *left = mergesort(a, n/2, sz, cmp);
  void *right = mergesort((const char*)a + sz*(int)(n/2), n-n/2, sz, cmp);
  void *all = merge(left, n/2, right, n-n/2, sz, cmp);
  free(left); free(right);
  return all;
}


void quicksort(void *a, size_t n, size_t sz, cmp_func_t cmp) {
  if (n <= 1) return;
  /* partition dataset choosing a random pivot (to avoid worst case n^2) */
  size_t p = (n-1) * random() / RAND_MAX;
  p = partition(a, n, p, sz, cmp);
  /* exclude the pivot and partition the new lists */
  quicksort(a, p, sz, cmp);
  quicksort((char*)a + (p+1)*sz, n - p-1, sz, cmp);
}


void quicksort3(void *a, size_t n, size_t sz, cmp_func_t cmp) {
  if (n <= 1) return;
  /* partition dataset choosing a random pivot (to avoid worst case n^2) */
  size_t p = (n-1) * random() / RAND_MAX;
  size_t equal_idx, greater_idx;
  partition3(a, n, p, &equal_idx, &greater_idx, sz, cmp);
  /* exclude equal values and partition the new lists */
  quicksort3(a, equal_idx, sz, cmp);
  quicksort3((char*)a + greater_idx*sz, n - greater_idx, sz, cmp);
}


/* select the n'th smallest element from a according to cmp */
void * selection(void *a, size_t n, size_t k, size_t sz, cmp_func_t cmp) {
  size_t b, c;
  size_t v = (n-1) * random() / RAND_MAX;
  partition3(a, n, v, &b, &c, sz, cmp);

  if (k < b) /* b is the idx for middle list. Search the smaller */
    return selection(a, b, k, sz, cmp);
  else if (k < c)
    return (char*)a + b*sz; /* pivot idx uppon partition return */
  else /* k > c, search the greater list */
    return selection((char*)a + c*sz, n-c, k-c, sz, cmp);
}

/* vim: set sw=2 sts=2 : */
