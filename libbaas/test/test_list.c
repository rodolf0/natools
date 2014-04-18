#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "baas/list.h"

int intcmp(const int *a, const int *b) {
  return *a > *b ? 1 : (*a < *b ? -1 : 0);
}
int intcmp_r(const int *a, const int *b) {
  return *a > *b ? -1 : (*a < *b ? 1 : 0);
}

list_t * generate_test_list(void) {
  size_t n = 0, i, j;
  int sum = 0, *e=NULL;
  list_t *l = list_init(free, (cmp_func_t)intcmp);
  list_node_t *ln;

  for (i = 0; i < 10000; i++) {
    switch (random() % 11) {
      case 0:
      case 1:
      case 2:
        e = (int*)malloc(sizeof(int)); *e = random() % 123456789;
        n++; sum += *e;
        list_push(l, e);
        break;
      case 3:
      case 4:
      case 5:
        e = (int*)malloc(sizeof(int)); *e = random() % 123456789;
        n++; sum += *e;
        list_queue(l, e);
        break;
      case 6:
      case 7:
        e = (int*)list_pop(l);
        if (e) { n--; sum -= *e; free(e); }
        break;
      case 8:
      case 9:
        e = (int*)list_dequeue(l);
        if (e) { n--; sum -= *e; free(e); }
        break;
      case 10:
        if (!list_size(l)) continue;
        j = random() % list_size(l);
        ln = list_first(l);
        while (j--)
          ln = list_node_next(ln);
        n--;
        sum -= *(int*)list_node_data(ln);
        list_remove(l, ln);
        break;
    }

    /* check pointers */
    if (list_size(l) > 0) {
      assert(list_first(l) != NULL);
      assert(list_last(l) != NULL);
      assert(list_node_prev(list_first(l)) == NULL);
      assert(list_node_next(list_last(l)) == NULL);
      /* just cause we know there's integers */
      assert(list_node_data(list_last(l)) != NULL);
      assert(list_node_data(list_first(l)) != NULL);
    }
    if (list_size(l) == 1) {
      assert(list_first(l) == list_last(l));
    }
  }

  /* check list threading */
  size_t forward = 0, backward = 0;
  int ctrlsum1 = 0, ctrlsum2 = 0;
  for (ln = list_first(l); ln; ln = list_node_next(ln))
  { forward++; ctrlsum1 += *(int*)list_node_data(ln); }
  for (ln = list_last(l); ln; ln = list_node_prev(ln))
  { backward++; ctrlsum2 += *(int*)list_node_data(ln); }

  /* check list size n elements */
  assert(n == forward);
  assert(n == backward);
  assert(n == list_size(l));
  /* check list element data */
  assert(sum == ctrlsum1);
  assert(sum == ctrlsum2);

  return l;
}


/* aux test functions */
int totalsum;
void acumulate(int *x) {
  totalsum += *x;
}
int * dupfunc(int *x) {
  int *r = (int*)malloc(sizeof(int));
  *r = *x * 2;
  return r;
}
int minimum;
void check_order(int *x) {
  assert(minimum <= *x);
  minimum = *x;
}

/* end of aux test functions*/


int test_looping(list_t *l) {
  int ctrlsum = 0;
  list_node_t *ln;

  for (ln = list_first(l); ln; ln = list_node_next(ln))
  { ctrlsum += *(int*)list_node_data(ln); }

  /* check foreach */
  totalsum = 0;
  list_foreach(l, (void (*)(void*))acumulate);
  assert(ctrlsum == totalsum);

  /* check map */
  totalsum = 0;
  list_t *two = (list_t*)list_map(l, (void *(*)(void *))dupfunc);
  list_foreach(two, (void (*)(void*))acumulate);
  two->free = free; two->cmp = (cmp_func_t)intcmp;
  assert(2 * ctrlsum == totalsum);

  /* check find */
  for (ln = list_first(l); ln; ln = list_node_next(ln)) {
    int f = *(int*)list_node_data(ln) * 2;
    assert(list_find(two, &f) != NULL);
  }

  list_destroy(two);
  return 0;
}

int test_listops(list_t *l) {
  size_t lsize = list_size(l);

  /* check duplication */
  list_t *l2 = list_dup(l);
  l2->free = NULL; /* avoid double free hereafter */
  assert(list_size(l2) == lsize);

  /* check splits n concats */
  list_t *a, *b;
  list_split_half(l2, &a, &b);
  assert(list_size(a) + list_size(b) == lsize);
  /* test duplication after split */
  list_t *c = list_dup(a);
  list_t *d = list_dup(b);
  assert(list_size(c) == list_size(a));
  assert(list_size(d) == list_size(b));
  /* check concatenation */
  l2 = list_concat(a, b);
  assert(list_size(l2) == lsize);

  /* check merge sort */
  l2 = list_mergesort(l2);
  minimum = 0;
  list_foreach(l2, (void (*)(void*))check_order);
  list_destroy(l2);

  c = list_mergesort(c);
  d = list_mergesort(d);
  minimum = 0; list_foreach(c, (void (*)(void*))check_order);
  minimum = 0; list_foreach(d, (void (*)(void*))check_order);

  /* check merge */
  l2 = list_merge(c, d);
  assert(list_size(l2) == lsize);
  minimum = 0; list_foreach(l2, (void (*)(void*))check_order);
  list_destroy(l2);

  return 0;
}


#define ITERATIONS 1000
int main(void) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    list_t *l = generate_test_list();
    test_looping(l);
    test_listops(l);
    list_destroy(l);
  }
  fprintf(stderr, "\n");

  return 0;
}

/* vim: set sw=2 sts=2 : */
