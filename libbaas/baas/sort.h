#ifndef _SORT_H_
#define _SORT_H_

typedef int (*cmp_func_t)(const void *, const void *);

/* merge: merge to list of elements (adjacent in memory) into a new list
 *        elements in each list are assumed to be sorted
 *        na/nb:   size of list a / b
 *        sz:    size of each element
 *        cmp:  comparison function */
void * merge(const void *a, size_t na,
             const void *b, size_t nb, size_t sz, cmp_func_t cmp);

/* Partition a list of elements into 2 sublists: one with elements
 * less or equal than the pivot and the other with bigger ones.
 * The k-th element is chosen as the pivot.
 * After partition the new position of the pivot is returned. */
size_t partition(void *a, size_t n, size_t kth, size_t sz, cmp_func_t cmp);
/* thighter inner loop than partition */
size_t partition2(void *a, size_t n, size_t kth, size_t sz, cmp_func_t cmp);
/* 3 way partition: smaller-a, equal-b, bigger-c */
void partition3(void *a, size_t n, int kth, size_t *b,
                size_t *c, size_t sz, cmp_func_t cmp);
void partition32(void *a, size_t n, int kth, size_t *b,
                 size_t *c, size_t sz, cmp_func_t cmp);

/* mergesort: sorts n elements of size s using cmp function to compare them.
 *  Elements must be adjacent in memory */
void * mergesort(const void *a, size_t n, size_t sz, cmp_func_t cmp);

/* quicksort: sort an array of contiguous elements
 *            quicksort3 uses a 3 way partition (no n^2 worst case) */
void quicksort(void *a, size_t n, size_t sz, cmp_func_t cmp);
void quicksort3(void *a, size_t n, size_t sz, cmp_func_t cmp);

/* selection: select the k-th smallest element
 *            eg: k=0 -> min, k=n/2 -> median */
void * selection(void *a, size_t n, size_t k, size_t sz, cmp_func_t cmp);

#endif

/* vim: set sw=2 sts=2 : */
