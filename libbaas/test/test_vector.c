#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../baas/vector.h"

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


vector_t * generate_test_vector() {
  vector_t *v = vector_init(free, (cmp_func_t)intcmp);
  int n = 0, i, sum = 0, *e;
  size_t idx;

  for (i = 0; i < 10000; i++) {
    switch (random() % 20) {
      case 0:
      case 1:
      case 2:
      case 3:
        idx = -(random() % (1+v->size));
        e = malloc(sizeof(int)); *e = random() % 123456789;
        n++; sum += *e;
        vector_insert(v, idx, e);
        break;
      case 4:
      case 5:
      case 6:
      case 7:
        idx = random() % (1+v->size);
        e = malloc(sizeof(int)); *e = random() % 123456789;
        n++; sum += *e;
        vector_insert(v, idx, e);
        break;
      case 8:
      case 9:
      case 10:
        if (!v->size) break;
        idx = random() % v->size;
        e = vector_get(v, idx);
        n--; sum -= *e;
        vector_remove(v, idx);
        break;
      case 11:
      case 12:
      case 13:
        if (!v->size) break;
        idx = -(random() % (1+v->size));
        e = vector_get(v, idx);
        n--; sum -= *e;
        vector_remove(v, idx);
        break;
      case 14:
      case 15:
      case 16:
        if (!v->size) break;
        idx = -(random() % (1+v->size));
        e = vector_get(v, idx);
        sum -= *e;
        e = malloc(sizeof(int)); *e = random() % 123456789;
        sum += *e;
        vector_set(v, idx, e);
        break;
      case 17:
      case 18:
      case 19:
        if (!v->size) break;
        idx = random() % v->size;
        e = vector_get(v, idx);
        sum -= *e;
        e = malloc(sizeof(int)); *e = random() % 123456789;
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
  vector_t *y = vector_copy(v);
  int i;
  for (i = 0; i < 100; i++) {
    int *search = vector_get(y, random() % y->size);
    int *find = vector_get(v, vector_find(v, search));
    if (search || find)
      assert(*search == *find);
  }
}

void test_resize(vector_t *v) {
  int j, n, sum;
  size_t idx = v->size * 75/100;

  totalsum = 0;
  n = v->size;
  vector_foreach(v, (void(*)(void*))acumulate);
  sum = totalsum;

  for (j = idx; j < v->size; j++) {
    n--; sum -= *(int*)vector_get(v, j);
  }
  vector_resize(v, idx);

  assert(n == v->size);
  assert(v->cap >= v->size);
  totalsum = 0;
  vector_foreach(v, (void(*)(void*))acumulate);
  assert(totalsum == sum);

  vector_resize(v, 2*idx);

  assert(2*idx == v->size);
  assert(v->cap >= v->size);
  totalsum = 0;
  vector_foreach(v, (void(*)(void*))acumulate);
  assert(totalsum == sum);
}


#define ITERATIONS 1000
int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    vector_t *v = generate_test_vector();
    test_search(v);
    test_resize(v);
    test_search(v);
    vector_destroy(v);
  }
  fprintf(stderr, "\n");

  return 0;
}

/* vim: set sw=2 sts=2 : */
