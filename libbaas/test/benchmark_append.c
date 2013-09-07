#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "baas/list.h"
#include "baas/vector.h"
#include "baas/bstree.h"
#include "baas/hashtbl.h"

int intcmp(const int *a, const int *b) {
  if (!a && !b)
    return 0;
  if (!a && b)
    return 1;
  if (a && !b)
    return -1;
  return *a > *b ? 1 : (*a < *b ? -1 : 0);
}

#define TEST_SZ (1 << 22)
int test_data[TEST_SZ];

/* return the number of usec between t0 and t1 */
int utime_diff(const struct timespec *t0, const struct timespec *t1) {
  return (t1->tv_sec - t0->tv_sec) * 1e6 +
         (t1->tv_nsec - t0->tv_nsec) / 1e3;
}

int benchmark_vector_append(void) {
  struct timespec tv_start, tv_end;
  int i;
  vector_t *v = vector_init(NULL, (cmp_func_t)intcmp);
  clock_gettime(CLOCK_MONOTONIC, &tv_start);
  for (i = 0; i < TEST_SZ; i++)
    vector_append(v, &test_data[i]);
  clock_gettime(CLOCK_MONOTONIC, &tv_end);
  vector_destroy(v);
  return utime_diff(&tv_start, &tv_end);
}

int benchmark_list_append(void) {
  struct timespec tv_start, tv_end;
  int i;
  list_t *v = list_init(NULL, (cmp_func_t)intcmp);
  clock_gettime(CLOCK_MONOTONIC, &tv_start);
  for (i = 0; i < TEST_SZ; i++)
    list_push(v, &test_data[i]);
  clock_gettime(CLOCK_MONOTONIC, &tv_end);
  list_destroy(v);
  return utime_diff(&tv_start, &tv_end);
}

int benchmark_bstree_append(void) {
  struct timespec tv_start, tv_end;
  int i;
  typedef struct {
    char key[32];
    int *value;
  } item_t;
  bstree_t *v = bstree_init((free_func_t)free, (cmp_func_t)intcmp, 1);
  clock_gettime(CLOCK_MONOTONIC, &tv_start);
  for (i = 0; i < TEST_SZ; i++) {
    item_t * d = (item_t*)malloc(sizeof(item_t));
    sprintf(d->key, "%d", test_data[i]);
    d->value = &test_data[i];
    bstree_insert(v, d);
  }
  clock_gettime(CLOCK_MONOTONIC, &tv_end);
  bstree_destroy(v);
  return utime_diff(&tv_start, &tv_end);
}

int benchmark_hashtbl_append(void) {
  struct timespec tv_start, tv_end;
  int i;
  hashtbl_t *v = hashtbl_init(NULL, (cmp_func_t)intcmp, 1);
  clock_gettime(CLOCK_MONOTONIC, &tv_start);
  for (i = 0; i < TEST_SZ; i++) {
    char key[32];
    sprintf(key, "%d", test_data[i]);
    hashtbl_insert(v, key, &test_data[i]);
  }
  clock_gettime(CLOCK_MONOTONIC, &tv_end);
  hashtbl_destroy(v);
  return utime_diff(&tv_start, &tv_end);
}

int main(void) {
  int i;
  for (i = 0; i < TEST_SZ; i++)
    test_data[i] = (int)random();

  int tv = benchmark_vector_append();
  int tl = benchmark_list_append();
  int tt = benchmark_bstree_append();
  int th = benchmark_hashtbl_append();

  fprintf(stderr, "Vector: %d, List: %d, BSTree: %d, HashTbl: %d\n",
          tv, tl, tt, th);
	return 0;
}

/* vim: set sw=2 sts=2 : */
