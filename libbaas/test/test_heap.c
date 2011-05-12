#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../baas/heap.h"

int intcmp(const int *a, const int *b) {
  return *a > *b ? 1 : (*a < *b ? -1 : 0);
}

void generate_test_heap() {
  int n = 0, i, j, sum = 0, *e=NULL;
  heap_t *h = heap_init(free, (cmp_func_t)intcmp);

  for (i = 0; i < 1000; i++) {
    switch (random() % 3) {
      case 0:
      case 2:
        e = malloc(sizeof(int)); *e = random() % 123456789;
        n++; sum += *e;
        heap_insert(h, e);
        break;
      case 1:
        e = heap_pop(h);
        if (e) { n--; sum -= *e; free(e); }
        break;
    }

    /* check propper auto resize */
    assert(h->bufsz >= h->size);
    for (j = 0; j < h->size; j++) {
      size_t left = heap_child_l(j);
      size_t right = heap_child_r(j);
      size_t greatest = heap_greatest_child(h, j);

      /* check for heap order is preserved and greatest child is correct */
      if (h->size > right) {
        assert(*(int*)h->data[j] >= *(int*)h->data[right]);
        assert(*(int*)h->data[greatest] >= *(int*)h->data[right]);
      }
      if (h->size > left) {
        assert(*(int*)h->data[j] >= *(int*)h->data[left]);
        assert(*(int*)h->data[greatest] >= *(int*)h->data[left]);
      }
    }
  }

  assert(h->size == n);
  int ctrlsum = 0, maxctrl;
  if (h->size) maxctrl = *(int*)h->data[0];
  while (h->size) {
    e = heap_pop(h); ctrlsum += *e;
    /* check that elements are in heap order */
    assert(*e <= maxctrl); maxctrl = *e;
    free(e);
  }
  assert(ctrlsum == sum);

  heap_destroy(h);
}

#define LOTS_OF_INTS 10000
void test_heapify() {
  size_t i;
  /* initialize data */
  int *array = malloc(sizeof(int) * LOTS_OF_INTS);
  int sum = 0;
  for (i = 0; i < LOTS_OF_INTS; i++) {
    array[i] = random() % 123456789;
    sum += array[i];
  }

  /* construct a heap from our random array */
  heap_t *h = heap_heapify(array, LOTS_OF_INTS, sizeof(int),
                           NULL, (cmp_func_t)intcmp);

  assert(h->size == LOTS_OF_INTS);
  int ctrlsum = 0, maxctrl, *e;
  /* peek head */
  if (h->size) maxctrl = *(int*)h->data[0];
  while (h->size) {
    e = heap_pop(h); ctrlsum += *e;
    /* check that elements are in heap order */
    assert(*e <= maxctrl); maxctrl = *e;
  }
  assert(ctrlsum == sum);
  heap_destroy(h);
  free(array);
}


#define ITERATIONS 1000
int main(int argc, char *argv[]) {
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
