#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "baas/list.h"

/******** Some aux functions for testing ***********/
int intcmp(const int *a, const int *b) {
  return *a > *b ? 1 : (*a < *b ? -1 : 0);
}
int intcmp_r(const int *a, const int *b) {
  return *a > *b ? -1 : (*a < *b ? 1 : 0);
}

int *_rint() {
  int *e = (int*)malloc(sizeof(int));
  *e = random() % 123456789;
  return e;
}

int _acum_sum = 0;
int _acum_count = 0;
void acumulate(int *x) {
  _acum_sum += *x;
  _acum_count++;
}

int * dupfunc(int *x) {
  int *r = (int*)malloc(sizeof(int));
  *r = *x * 2;
  return r;
}

/**************************************************/


void assert_list(list_t *l) {
  if (!l || list_size(l) == 0) return;

  assert(list_first(l) != NULL);
  assert(list_last(l) != NULL);
  assert(list_prev(list_first(l)) == NULL);
  assert(list_next(list_last(l)) == NULL);

  size_t forward = 0, backward = 0;
  for (list_node_t *n = list_first(l); n; n = list_next(n))
    forward++;
  for (list_node_t *n = list_last(l); n; n = list_prev(n))
    backward++;
  assert(list_size(l) == forward);
  assert(list_size(l) == backward);

  assert(list_nth(l, 1) == list_first(l));
  assert(list_nth(l, list_size(l)) == list_last(l));
  assert(list_next(list_first(l)) == list_nth(l, 2));
  assert(list_prev(list_last(l)) == list_nth(l, list_size(l)-1));

  assert(*(int*)list_peek_head(l) == *(int*)list_data(list_first(l)));
  assert(*(int*)list_peek_tail(l) == *(int*)list_data(list_last(l)));
}

void assert_integers(list_t *l) {
  int sum1 = 0, sum2 = 0;
  for (list_node_t *n = list_first(l); n; n = list_next(n))
    sum1 += *(int*)list_data(n);
  for (list_node_t *n = list_last(l); n; n = list_prev(n))
    sum2 += *(int*)list_data(n);
  assert(sum1 == sum2);
}

list_t * generate_test_list(size_t size) {
  list_t *l = list_init(free, (cmp_func_t)intcmp);
  int sum = 0, count = 0;
  while (list_size(l) < size) {
    /* flip a coin to exersice different insertion ops */
    int choice = random() % 100;
    if (choice < 20) {
      int *e = _rint();
      sum += *e;
      count++;
      assert(list_push(l, e));
    } else if (choice < 40) {
      int *e = _rint();
      sum += *e;
      count++;
      assert(list_queue(l, e));
    } else if (choice < 70) {
      int *e = _rint();
      sum += *e;
      count++;
      int pos = random() % (2 * (1 + list_size(l)));
      list_node_t *nth = list_nth(l, pos); /* can be NULL */
      assert(list_insert(l, nth, e));
    } else if (choice < 80) {
      int *e = list_pop(l);
      if (e) {
        sum -= *e;
        count--;
        free(e);
      }
    } else if (choice < 90) {
      int *e = list_dequeue(l);
      if (e) {
        sum -= *e;
        count--;
        free(e);
      }
    } else if (choice < 100 && list_size(l) > 0) {
      int pos = 1 + random() % list_size(l);
      list_node_t *nth = list_nth(l, pos);
      /* check list_nth */
      list_node_t *chk = list_first(l);
      while (--pos) chk = list_next(chk);
      assert(chk == nth);
      assert(list_data(nth) == list_data(chk));
      sum -= *(int*)list_data(nth);
      count--;
      list_remove(l, nth);
    }

    /* just cause we know there's integers */
    if (list_size(l) > 0) {
      assert(list_data(list_last(l)) != NULL);
      assert(list_data(list_first(l)) != NULL);
    }
    if (list_size(l) == 1) {
      assert(list_first(l) == list_last(l));
    }
  }
  return l;
}


void test_foreach(list_t *l) {
  int ctrlsum = 0, ctrlcount = 0;
  for (list_node_t *n = list_first(l); n; n = list_next(n)) {
    ctrlsum += *(int*)list_data(n);
    ctrlcount++;
  }
  _acum_count = _acum_sum = 0;
  list_foreach(l, (void (*)(void*))acumulate);
  assert(_acum_count == ctrlcount);
  assert(_acum_sum == ctrlsum);
}

void test_map(list_t *l) {
  int ctrlsum = 0, ctrlcount = 0;
  for (list_node_t *n = list_first(l); n; n = list_next(n)) {
    ctrlsum += *(int*)list_data(n);
    ctrlcount++;
  }
  list_t *two = (list_t*)list_map(l, (void *(*)(void *))dupfunc);
  list_set_free(two, free);
  int sum = 0, count = 0;
  for (list_node_t *n = list_first(two); n; n = list_next(n)) {
    sum += *(int*)list_data(n);
    count++;
  }
  assert(list_size(two) == list_size(l));
  assert(count == ctrlcount);
  assert(sum == 2 * ctrlsum);
  list_destroy(two);
}

void test_find(list_t *l) {
  list_node_t *n = list_nth(l, 1 + random() % list_size(l));
  list_node_t *f = list_find(l, list_data(n));
  assert(f != NULL);
  assert(*(int*)list_data(f) == *(int*)list_data(n));
}

void test_dup(list_t *l) {
  list_t *l2 = list_dup(l);
  assert(list_size(l2) == list_size(l));
  list_node_t *n = list_first(l), *n2 = list_first(l2);
  do {
    assert(n != n2);
    assert(*(int*)list_data(n) == *(int*)list_data(n2));
    n = list_next(n);
    n2 = list_next(n2);
  } while (n && n2);
  list_destroy(l2);
}

void test_concat(list_t *l) {
  size_t s2 = 1 + list_size(l)/3;
  list_t *l1 = list_dup(l);
  list_t *l2 = generate_test_list(s2);
  /* test concat with null */
  list_t *c1 = list_concat(l1, NULL);
  assert(c1 == l1);
  l1 = NULL;
  list_t *c2 = list_concat(NULL, l2);
  assert(c2 == l2);
  l2 = NULL;
  /* test concat with self */
  list_t *c3 = list_concat(c1, c1);
  c1 = NULL;
  assert(list_size(c3) == 2 * list_size(l));
  assert_list(c3);
  list_destroy(c3);
  c3 = NULL;
  /* test 2 list concat */
  list_t *l3 = generate_test_list(s2);
  size_t s = list_size(c2) + list_size(l3);
  list_t *c4 = list_concat(c2, l3);
  c2 = l3 = NULL;
  assert_list(c4);
  assert(list_size(c4) == s);
  list_destroy(c4);
}

void test_split(list_t *l) {
  list_t *l2 = list_dup(l);
  size_t split = list_size(l2) / 2;
  list_t *a, *b, *c, *d, *e, *f;
  list_split(l2, split, &a, &b);
  l2 = NULL;
  assert(list_size(a) == split);
  assert(list_size(b) == list_size(l) - split);
  assert_list(a);
  assert_list(b);
  /* 0 elements in first */
  list_split(a, 0, &c, &d);
  a = NULL;
  assert(c && list_size(c) == 0);
  assert(d && list_size(d) == split);
  /* all elements in first */
  list_split(b, list_size(l) - split, &e, &f);
  b = NULL;
  assert(e && (list_size(e) == list_size(l) - split));
  assert(f && list_size(f) == 0);
  assert_list(c);
  assert_list(d);
  assert_list(e);
  assert_list(f);
  list_destroy(c);
  list_destroy(d);
  list_destroy(e);
  list_destroy(f);
}

void test_merge_sort(list_t *l) {
  size_t os = list_size(l);
  /* sort from max to min */
  list_set_cmp(l, (cmp_func_t)intcmp_r);
  l = list_mergesort(l);
  assert_list(l);
  int _maximum = *(int*)list_data(list_first(l));
  for (list_node_t *n = list_first(l); n; n = list_next(n)) {
    int i = *(int*)list_data(n);
    assert(_maximum >= i);
    _maximum = i;
  }
  /* sort from min to max */
  list_set_cmp(l, (cmp_func_t)intcmp);
  l = list_mergesort(l);
  assert_list(l);
  int _minimum = *(int*)list_data(list_first(l));
  for (list_node_t *n = list_first(l); n; n = list_next(n)) {
    int i = *(int*)list_data(n);
    assert(_minimum <= i);
    _minimum = i;
  }
  assert(list_size(l) == os);
  /* test merging */
  list_t *l2 = list_mergesort(generate_test_list(os));
  l = list_merge(l, l2);
  assert(list_size(l) == 2 * os);
  assert_list(l);
   _minimum = *(int*)list_data(list_first(l));
  for (list_node_t *n = list_first(l); n; n = list_next(n)) {
    int i = *(int*)list_data(n);
    assert(_minimum <= i);
    _minimum = i;
  }
}


#define ITERATIONS 1000
int main(void) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    size_t ls = 1 + random() % (2 * i);
    fprintf(stderr, "\rtesting size=%-6zu ... %3d%%", ls, 100 * i / ITERATIONS);
    list_t *l = generate_test_list(ls);
    assert(list_size(l) > 0);
    assert_list(l);
    assert_integers(l);

    test_foreach(l);
    test_map(l);
    test_find(l);

    test_dup(l);
    test_concat(l);
    test_split(l);

    test_merge_sort(l);

    list_destroy(l);
  }
  fprintf(stderr, "\n");

  return 0;
}

/* vim: set sw=2 sts=2 : */
