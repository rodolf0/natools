#include <stdlib.h>
#include <string.h>
#include "baas/sort.h"

/* get to offset i of size sz from a */
#define IDX(a, i) (a + sz * (i))
/* use a previously alloc'ed t to swap contents */
#define SWAP(x, y) do { \
  memcpy(t, (x), sz); memcpy((x), y, sz); memcpy((y), t, sz); } while(0)

size_t partition(void *a, size_t n, size_t kth, size_t sz, cmp_func_t cmp) {
  /* move the pivot out of the way (swap it with last element) */
  void *t = malloc(sz);
  SWAP(IDX(a, kth), IDX(a, n-1));
  /* iterate until you find an item smaller than the pivot (a+n-1)
   * exchange that into the current store lot. */
  size_t i, store_idx = 0;
  for (i = 0; i < n-1; i++) {
    if (cmp(IDX(a, i), IDX(a, n-1)) <= 0) {
      /* swap smaller item with the current store slot */
      SWAP(IDX(a, store_idx), IDX(a, i));
      store_idx++;
    }
  }
  /* swap pivot to definite store idx */
  SWAP(IDX(a, n-1), IDX(a, store_idx));
  free(t);
  return store_idx;
}

/* Identical behaviour to partition but faster inner loop? */
size_t partition2(void *a, size_t n, size_t kth, size_t sz, cmp_func_t cmp) {
  ssize_t l = -1, r = n-1;
  void *t = malloc(sz);
  /* put the pivot out of the way (at last position) */
  SWAP(IDX(a, kth), IDX(a, n-1));
  while (l < r) {
    /* search for element bigger and lesser thatn the pivot to exchange */
    /* ',' has left-to-right assoc => avoiding --/++ in macro expansion */
    while (++l, cmp(IDX(a, l), IDX(a, n-1)) <= 0 && l < r);
    /* 1st --r skips pivot. */
    while (--r, cmp(IDX(a, r), IDX(a, n-1)) > 0 && l < r);
    if (l >= r) break;
    /* exchange */
    SWAP(IDX(a, l), IDX(a, r));
  }
  /* move the pivot to it's final place (where l ends) */
  SWAP(IDX(a, n-1), IDX(a, l));
  free(t);
  return l;
}

void partition3(void *a, size_t n, int kth, size_t *b,
                size_t *c, size_t sz, cmp_func_t cmp) {
  size_t lt_idx = 0, eq_idx = n-1;
  ssize_t i = 0; void *t = malloc(sz);
  /* move the pivot out of the way (swap it with last element) */
  SWAP(IDX(a, kth), IDX(a, n-1));
  /* reorder array */
  while (i < eq_idx) {
    /* if a[i] < pivot put it at the begining */
    if (cmp(IDX(a, i), IDX(a, n-1)) < 0)
    { if (i > lt_idx) SWAP(IDX(a, i), IDX(a, lt_idx)); i++; lt_idx++; }
    /* if a[i] == pivot put it at the end with all pivots */
    else if (cmp(IDX(a, i), IDX(a, n-1)) == 0)
    { eq_idx--; SWAP(IDX(a, i), IDX(a, eq_idx)); }
    /* else: a[i] > pivot ... leave it in the middle */
    else i++;
  }
  /* swap pivots back to the middle or greater elems to the end  */
  i = (n-1 - eq_idx) < (eq_idx - lt_idx) ? (n-1 - eq_idx) : (eq_idx - lt_idx);
  while (i >= 0) /* >= 0 to bring back the pivot */
  { SWAP(IDX(a, n-1-i), IDX(a, lt_idx+i)); i--; }
  /* set return vals */
  *b = lt_idx; *c = n - (eq_idx - lt_idx);
  free(t);
}

/* Similar to partition but parition into 3 lists: smaller, equal, bigger
 * On return, a points to beggining, b to equal list, c to greater */
void partition32(void *a, size_t n, int kth, size_t *b,
                size_t *c, size_t sz, cmp_func_t cmp) {
  ssize_t l = -1, r = n-1;
  ssize_t p = l, q = r;
  void *t = malloc(sz);
  /* move the pivot out of the way (at last position) */
  SWAP(IDX(a, kth), IDX(a, n-1));

  while (l < r) {
    /* search for element bigger and lesser thatn the pivot to exchange */
    while (++l, cmp(IDX(a, l), IDX(a, n-1)) < 0 && l < r);
    while (--r, cmp(IDX(a, n-1), IDX(a, r)) < 0 && l < r);
    if (l < r) {
      /* exchange */
      SWAP(IDX(a, l), IDX(a, r));
      /* items equal to the pivot are pushed on the edges to later join them */
      if (cmp(IDX(a, l), IDX(a, n-1)) == 0) { p++; SWAP(IDX(a, p), IDX(a, l)); }
    }
    if (l <= r )
      if (cmp(IDX(a, r), IDX(a, n-1)) == 0) { q--; SWAP(IDX(a, q), IDX(a, r)); }
  }
  /* l points to the final pivot position, move it to it's final place */
  SWAP(IDX(a, l), IDX(a, n-1));   r = l;
  /* move equal keys on the extremes to the middle. l marks the pivot */
  while (p >= 0) /* r-- to skip pivot, p-- advance element */
  { r--; SWAP(IDX(a, p), IDX(a, r)); p--; }
  while (q < n-1)
  { l++; SWAP(IDX(a, q), IDX(a, l)); q++; }
  /* set the start of equal and greater list */
  *b = r; *c = l+1;
  free(t);
}

/* merge: merge to list of elements (adjacent in memory) into a new list
 *        elements in each list are assumed to be sorted
 *        na/nb:   size of list a / b
 *        sz:    size of each element
 *        cmp:  comparison function */
void * merge(const void *a, size_t na,
             const void *b, size_t nb,
             size_t sz, cmp_func_t cmp) {
  /* return list */
  void *r = malloc((na + nb) * sz);
  void *c = NULL; /* sub mergeed list */

  if (na == 0) {
    memcpy(r, b, nb * sz);
  } else if (nb == 0) {
    memcpy(r, a, na * sz);
  } else {
    /* get smallest element on to the result array then keep on merging */
    if (cmp(a, b) < 0) {
      memcpy(r, a, sz);
      /* skip the first element, and merge rest */
      c = merge(a+sz, na-1, b, nb, sz, cmp);
    } else {
      memcpy(r, b, sz);
      c = merge(a, na, b+sz, nb-1, sz, cmp);
    }
    /* concat the just-merged part */
    memcpy(r+sz, c, (na+nb-1) * sz);
    if (c) free(c);
  }
  return r;
}

#undef IDX
#undef SWAP

/* vim: set sw=2 sts=2 : */
