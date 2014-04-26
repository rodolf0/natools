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

/**************************************************/
vector_t * generate_test_vector(size_t size) {
  vector_t *v = vector_init(free, (cmp_func_t)intcmp);
  int sum = 0;
  size_t count = 0;
  while (vector_size(v) < size) {
    /* flip a coin to exersice different insertion ops */
    int choice = random() % 100;
    if (choice < 80) {
      int *e = _rint();
      int idx = random() % (1+vector_size(v));
      idx = (choice < 40) ? -idx : idx;
      sum += *e; count++;
      assert(v = vector_insert(v, idx, e));
      if (idx >= 0)
        assert(vector_get(v, idx) == e);
      else
        assert(vector_get(v, vector_size(v) + idx - 1) == e);
    } else if (choice < 100 && vector_size(v) > 0) {
      int idx = random() % vector_size(v);
      idx = (choice < 90) ? -idx : idx;
      int *e = (int*)vector_get(v, idx);
      sum -= *e; count--;
      assert(v = vector_remove(v, idx));
    }
  }
  assert(count == vector_size(v));
  assert(vector_capacity(v) >= vector_size(v));
  int totalsum = 0;
  for (size_t i = 0; i < vector_size(v); ++i)
    totalsum += *(int*)vector_get(v, i);
  assert(totalsum == sum);
  return v;
}

void test_search(vector_t *v) {
  for (int i = 0; i < 100; ++i) {
    int *needle = (int*)vector_get(v, random() % vector_size(v));
    if (!needle) continue;
    ssize_t idx = vector_find(v, needle);
    if (idx >= 0) {
      assert(*(int*)vector_get(v, idx) == *needle);
    }
  }
}


#define ITERATIONS 1000
int main(void) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    size_t vs = 1 + random() % (2 * i);
    fprintf(stderr, "\rtesting size=%-6zu ... %3d%%", vs, 100 * i / ITERATIONS);
    vector_t *v = generate_test_vector(vs);
    test_search(v);
    vector_destroy(v);
  }
  fprintf(stderr, "\n");
  return 0;
}

/* vim: set sw=2 sts=2 : */
