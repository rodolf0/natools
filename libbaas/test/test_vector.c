#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "baas/vector.h"

int intcmp(const int *a, const int *b) {
  if (!a && !b)
    return 0;
  if (!a && b)
    return 1;
  if (a && !b)
    return -1;

  return *a > *b ? 1 : (*a < *b ? -1 : 0);
}

/* aux test functions */
int totalsum;
void acumulate(int *x) {
  if (x)
    totalsum += *x;
}


vector_t * generate_test_vector(void) {
  vector_t *v = vector_init(free, (cmp_func_t)intcmp);
  size_t n = 0, i, idx;
  int sum = 0, *e;

  for (i = 0; i < 10000; i++) {
    switch (random() % 20) {
      case 0:
      case 1:
      case 2:
      case 3:
        idx = -(random() % (1+v->size));
        e = (int*)malloc(sizeof(int)); *e = random() % 123456789;
        n++; sum += *e;
        vector_insert(v, idx, e);
        break;
      case 4:
      case 5:
      case 6:
      case 7:
        idx = random() % (1+v->size);
        e = (int*)malloc(sizeof(int)); *e = random() % 123456789;
        n++; sum += *e;
        vector_insert(v, idx, e);
        break;
      case 8:
      case 9:
      case 10:
        if (!v->size) break;
        idx = random() % v->size;
        e = (int*)vector_get(v, idx);
        n--; sum -= *e;
        vector_remove(v, idx);
        break;
      case 11:
      case 12:
      case 13:
        if (!v->size) break;
        idx = -(random() % (1+v->size));
        e = (int*)vector_get(v, idx);
        n--; sum -= *e;
        vector_remove(v, idx);
        break;
      case 14:
      case 15:
      case 16:
        if (!v->size) break;
        idx = -(random() % (1+v->size));
        e = (int*)vector_get(v, idx);
        sum -= *e;
        e = (int*)malloc(sizeof(int)); *e = random() % 123456789;
        sum += *e;
        vector_set(v, idx, e);
        break;
      case 17:
      case 18:
      case 19:
        if (!v->size) break;
        idx = random() % v->size;
        e = (int*)vector_get(v, idx);
        sum -= *e;
        e = (int*)malloc(sizeof(int)); *e = random() % 123456789;
        sum += *e;
        vector_set(v, idx, e);
        break;
    }
  }

  assert(n == v->size);
  assert(v->cap >= v->size);
  totalsum = 0;
  vector_foreach(v, (void(*)(void*))acumulate);
  assert(totalsum == sum);

  return v;
}

void test_search(vector_t *v) {
  int i;
  for (i = 0; i < 100; i++) {
    int *search = (int*)vector_get(v, random() % v->size);
    if (!search) continue; /* after resize lots of null elems */
    int *find = (int*)vector_get(v, vector_find(v, search));
    assert(*search == *find);
  }
}

void test_resize(vector_t *v) {
  size_t j, newsize = v->size * (60 + random() % 10) / 100;
  int sum = 0;

  for (j = 0; j < newsize; j++) {
    sum += *(int*)vector_get(v, j);
  }
  vector_resize(v, newsize);

  assert(newsize == v->size);
  assert(v->cap >= v->size);
  totalsum = 0;
  vector_foreach(v, (void(*)(void*))acumulate);
  /*fprintf(stderr, "%d, %d\n", totalsum, sum);*/
  assert(totalsum == sum);

  vector_resize(v, 2*newsize);

  assert(2*newsize == v->size);
  assert(v->cap >= v->size);
  totalsum = 0;
  vector_foreach(v, (void(*)(void*))acumulate);
  assert(totalsum == sum);
}


#define ITERATIONS 1000
int main(void) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    vector_t *v = generate_test_vector();
    test_search(v);
    test_resize(v);
    if (v->size > 0)
      test_search(v);
    vector_destroy(v);
  }
  fprintf(stderr, "\n");

  return 0;
}

/* vim: set sw=2 sts=2 : */
