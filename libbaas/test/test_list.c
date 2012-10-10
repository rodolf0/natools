#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../baas/list.h"

int intcmp(const int *a, const int *b) {
  return *a > *b ? 1 : (*a < *b ? -1 : 0);
}
int intcmp_r(const int *a, const int *b) {
  return *a > *b ? -1 : (*a < *b ? 1 : 0);
}

list_t * generate_test_list() {
  int n = 0, i, j, sum = 0, *e=NULL;
  list_t *l = list_init(free, (cmp_func_t)intcmp);
  list_node_t *ln;

  for (i = 0; i < 10000; i++) {
    switch (random() % 11) {
      case 0:
      case 1:
      case 2:
        e = malloc(sizeof(int)); *e = random() % 123456789;
        n++; sum += *e;
        list_push(l, e);
        break;
      case 3:
      case 4:
      case 5:
        e = malloc(sizeof(int)); *e = random() % 123456789;
        n++; sum += *e;
        list_queue(l, e);
        break;
      case 6:
      case 7:
        e = list_pop(l);
        if (e) { n--; sum -= *e; free(e); }
        break;
      case 8:
      case 9:
        e = list_dequeue(l);
        if (e) { n--; sum -= *e; free(e); }
        break;
      case 10:
        if (!l->size) continue;
        j = random() % l->size;
        ln = l->first;
        while (j--)
          ln = ln->next;
        n--;
        sum -= *(int*)ln->data;
        list_remove(l, ln);
        break;
    }

    /* check pointers */
    if (l->size > 0) {
      assert(l->first != NULL);
      assert(l->last != NULL);
      assert(l->first->prev == NULL);
      assert(l->last->next == NULL);
      /* just cause we know there's integers */
      assert(l->last->data != NULL);
      assert(l->first->data != NULL);
    }
    if (l->size == 1) {
      assert(l->first == l->last);
    }
  }

  /* check list threading */
  int forward = 0, backward = 0, ctrlsum1 = 0, ctrlsum2 = 0;
  for (ln = l->first; ln; ln = ln->next)
  { forward++; ctrlsum1 += *(int*)ln->data; }
  for (ln = l->last; ln; ln = ln->prev)
  { backward++; ctrlsum2 += *(int*)ln->data; }

  /* check list size n elements */
  assert(n == forward);
  assert(n == backward);
  assert(n == l->size);
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
  int *r = malloc(sizeof(int));
  *r = *x * 2;
  return r;
}
/* end of aux test functions*/


int test_looping(list_t *l) {
  int ctrlsum = 0;
  list_node_t *ln;

  for (ln = l->first; ln; ln = ln->next)
  { ctrlsum += *(int*)ln->data; }

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
  for (ln = l->first; ln; ln = ln->next) {
    int f = *(int*)ln->data * 2;
    assert(list_find(two, &f) != NULL);
  }

  list_destroy(two);
  return 0;
}

int test_listops(list_t *l) {
  int lsize = l->size;

  /* check duplication */
  list_t *a, *b;
  list_t *l2 = list_dup(l);
  l2->free = NULL; /* avoid double free hereafter */
  assert(l2->size == lsize);

  /* check splits n concats */
  list_split_half(l2, &a, &b);
  assert(a->size + b->size == lsize);
  /* test duplication after split */
  list_t *c = list_dup(a);
  list_t *d = list_dup(b);
  assert(c->size + d->size == lsize);
  assert(c->size == a->size);
  assert(d->size == b->size);
  /* check concatenation */
  l2 = list_concat(a, b);
  assert(l2->size == lsize);
  /* check re-split */
  list_split_half(l2, &a, &b);
  assert(a->size + b->size == lsize);
  assert(a->size == c->size);
  assert(b->size == d->size);

  /* check merge */
  l2 = list_merge(a, b);
  assert(l2->size == lsize);
  int csize = c->size, dsize = d->size;
  l2 = list_merge(l2, c);
  assert(l2->size == lsize + csize);
  l2 = list_merge(l2, d);
  assert(l2->size == lsize + csize + dsize);

  /* check merge sort */
  l2 = list_mergesort(l2);
  assert(l2->size == lsize + csize + dsize);
  int prev = (l2->first ? *(int*)l2->first->data : 0);
  list_node_t *ln;
  for (ln = l2->first; ln; ln = ln->next) {
    assert(*(int*)ln->data >= prev);
    prev = *(int*)ln->data;
  }
  /* reverse sort order */
  l2->cmp = (cmp_func_t)intcmp_r;
  l2 = list_mergesort(l2);
  assert(l2->size == lsize + csize + dsize);
  prev = (l2->first ? *(int*)l2->first->data : 223456789);
  for (ln = l2->first; ln; ln = ln->next) {
    assert(*(int*)ln->data <= prev);
    prev = *(int*)ln->data;
  }

  list_destroy(l2);
  return 0;
}


#define ITERATIONS 1000
int main(int argc, char *argv[]) {
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
