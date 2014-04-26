#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "baas/vector.h"

/******** Some aux functions for testing ***********/
int intcmp(const int *a, const int *b) {
  if (!a && !b)
    return 0;
  if (!a && b)
    return 1;
  if (a && !b)
    return -1;
  return *a > *b ? 1 : (*a < *b ? -1 : 0);
}

int *_rint() {
  int *e = (int*)zmalloc(sizeof(int));
  *e = random() % 123456789;
  return e;
}

int _acum_sum = 0;
int _acum_count = 0;
void acumulate(int *x) {
  _acum_sum += *x;
  _acum_count++;
}

/* unsafe: asumes knowledge about underlying data */
void assert_vector_internals(vector_t *v) {
  int **data = (int**)vector_raw(v);
  for (size_t i = vector_size(v); i < vector_capacity(v); ++i)
    assert(data[i] == NULL);
  for (size_t i = 0; i < vector_size(v); ++i)
    assert(vector_get(v, i) == data[i]);
}

/**************************************************/
vector_t * generate_test_vector(size_t size) {
  vector_t *v = vector_init(free, (cmp_func_t)intcmp);
  int sum = 0;
  size_t count = 0;
  while (vector_size(v) < size) {
    /* flip a coin to exersice different insertion ops */
    int choice = random() % 100;
    if (choice < 70) {
      int *e = _rint();
      int idx = random() % (1+vector_size(v));
      idx = (choice < 35) ? -idx : idx;
      sum += *e; count++;
      assert(v = vector_insert(v, idx, e));
      if (idx >= 0)
        assert(vector_get(v, idx) == e);
      else
        assert(vector_get(v, vector_size(v) + idx - 1) == e);
    } else if (choice < 80 && vector_size(v) > 0) {
      int idx = random() % vector_size(v);
      idx = (choice < 75) ? -idx : idx;
      sum -= *(int*)vector_get(v, idx);
      int *g = _rint();
      sum += *g;
      vector_set(v, idx, g); /* will free prev elem */
    } else if (choice < 100 && vector_size(v) > 0) {
      int idx = random() % vector_size(v);
      idx = (choice < 90) ? -idx : idx;
      int *e = (int*)vector_get(v, idx);
      sum -= *e; count--;
      assert(v = vector_remove(v, idx));
    }
  }
  assert_vector_internals(v);
  assert(count == vector_size(v));
  assert(vector_capacity(v) >= vector_size(v));
  int totalsum = 0;
  for (size_t i = 0; i < vector_size(v); ++i)
    totalsum += *(int*)vector_get(v, i);
  assert(totalsum == sum);
  return v;
}

void test_search(vector_t *v) {
  int i = 100;
  while (i >= 0) {
    int *needle = (int*)vector_get(v, random() % vector_size(v));
    ssize_t idx = vector_find(v, needle);
    /* we should always be able to deref our ints */
    assert(*(int*)vector_get(v, idx) == *needle);
    --i;
  }
}

void test_foreach(vector_t *v) {
  int ctrlsum = 0, ctrlcount = 0;
  for (size_t n = 0; n < vector_size(v); ++n) {
    ctrlsum += *(int*)vector_get(v, n);
    ctrlcount++;
  }
  _acum_count = _acum_sum = 0;
  vector_foreach(v, (void (*)(void*))acumulate);
  assert(_acum_count == ctrlcount);
  assert(_acum_sum == ctrlsum);
}

#define ITERATIONS 1000
int main(void) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    size_t vs = 1 + random() % (2 * i);
    fprintf(stderr, "\rtesting size=%-6zu ... %3d%%", vs, 100 * i / ITERATIONS);
    vector_t *v = generate_test_vector(vs);
    test_search(v);
    test_foreach(v);
    vector_destroy(v);
  }
  fprintf(stderr, "\n");
  return 0;
}

/* vim: set sw=2 sts=2 : */
