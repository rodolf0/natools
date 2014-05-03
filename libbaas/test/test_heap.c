#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "baas/heap.h"

int intcmp(const int *a, const int *b) {
  return *a > *b ? 1 : (*a < *b ? -1 : 0);
}

void generate_test_heap(void) {
  size_t n = 0, i, j;
  int sum = 0, *e=NULL;
  heap_t *h = heap_init(free, (cmp_func_t)intcmp);

  for (i = 0; i < 1000; i++) {
    switch (random() % 3) {
      case 0:
      case 2:
        e = zmalloc(sizeof(int)); *e = random() % 123456789;
        n++; sum += *e;
        heap_insert(h, e);
        break;
      case 1:
        e = heap_pop(h);
        if (e) { n--; sum -= *e; free(e); }
        break;
    }

    /* check propper auto resize */
    assert(heap_capacity(h) >= heap_size(h));
    for (j = 0; j < heap_size(h); j++) {
      size_t left = heap_child_l(j);
      size_t right = heap_child_r(j);
      size_t greatest = heap_greatest_child(h, j);

      /* check for heap order is preserved and greatest child is correct */
      if (heap_size(h) > right) {
        assert(*(int*)heap_get(h, j) >= *(int*)heap_get(h, right));
        assert(*(int*)heap_get(h, greatest) >= *(int*)heap_get(h, right));
      }
      if (heap_size(h) > left) {
        assert(*(int*)heap_get(h, j) >= *(int*)heap_get(h, left));
        assert(*(int*)heap_get(h, greatest) >= *(int*)heap_get(h, left));
      }
    }
  }

  assert(heap_size(h) == n);
  int ctrlsum = 0, maxctrl;
  if (heap_size(h)) maxctrl = *(int*)heap_get(h, 0);
  while (heap_size(h)) {
    e = heap_pop(h); ctrlsum += *e;
    /* check that elements are in heap order */
    assert(*e <= maxctrl); maxctrl = *e;
    free(e);
  }
  assert(ctrlsum == sum);

  heap_destroy(h);
}

#define LOTS_OF_INTS 1000
void test_heapify(void) {
  size_t i;
  /* initialize data */
  int *array = zmalloc(sizeof(int) * LOTS_OF_INTS);
  int sum = 0;
  for (i = 0; i < LOTS_OF_INTS; i++) {
    array[i] = random() % 123456789;
    sum += array[i];
  }

  /* construct a heap from our random array */
  heap_t *h = heap_heapify(array, LOTS_OF_INTS, sizeof(int),
                           NULL, (cmp_func_t)intcmp);

  assert(heap_size(h) == LOTS_OF_INTS);
  int ctrlsum = 0, maxctrl, *e;
  /* peek head */
  if (heap_size(h)) maxctrl = *(int*)heap_get(h, 0);
  while (heap_size(h)) {
    e = heap_pop(h); ctrlsum += *e;
    /* check that elements are in heap order */
    assert(*e <= maxctrl); maxctrl = *e;
  }
  assert(ctrlsum == sum);
  heap_destroy(h);
  free(array);
}


#define ITERATIONS 1000
int main(void) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    generate_test_heap();
    test_heapify();
  }
  fprintf(stderr, "\n");

  return 0;
}

/* vim: set sw=2 sts=2 : */
