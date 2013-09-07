#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "baas/sort.h"

/* mostly usefull for argv[...] */
int str_cmp(char **a, char **b) {
  return strcmp(*a, *b);
}
int int_cmp(const int *a, const int *b) {
  return (*a == *b ? 0 : *a < *b ? -1 : 1);
}

/****** Data generation ********/
int * generate_random_ints(int n) {
  int i, *r = (int*)malloc(sizeof(int) * n);
  for (i = 0; i < n; i++)
    r[i] = random() % 1024;
  return r;
}

void free_random_ints(int *r) {
  free(r);
}

char ** generate_random_strings(int n) {
  int i, j;
  const char *alphanums = "abcdefghijklmnopqrstuvwxyz1234567890";
  int alphalen = strlen(alphanums);

  char **r = (char**)malloc(sizeof(char*) * n);
  for (i = 0; i < n; i++) {
    int len = 1 + random() % 255;
    r[i] = (char*)malloc(sizeof(char) * len);
    for (j = 0; j < len; j++)
      r[i][j] = alphanums[random() % alphalen];
    r[i][len-1] = '\0';
  }
  return r;
}

void free_random_strings(char **s, int n) {
  int i;
  for (i = 0; i < n; i++)
    free(s[i]);
  free(s);
}
/*******************************/

/* check elements are truly ordered */
void check_ordered(void *elems, int n, int sz, cmp_func_t cmp) {
  int j; void *min = (char*)elems+0*sz;
  for (j = 0; j < n; j++) {
    assert(cmp(min, (char*)elems+j*sz) <= 0);
    min = (char*)elems+j*sz;
  }
}

#define NUM_ELEMS 1000
void test_mergesort(void) {
  /* test with ints */
  int *ints = generate_random_ints(NUM_ELEMS);
  int *sorted_i = (int*)mergesort(ints, NUM_ELEMS, sizeof(int), (cmp_func_t)int_cmp);
  check_ordered(sorted_i, NUM_ELEMS, sizeof(int), (cmp_func_t)int_cmp);
  free(sorted_i);
  free_random_ints(ints);
  /* test with chars */
  char **chars = generate_random_strings(NUM_ELEMS);
  char **sorted_c = (char**)mergesort(chars, NUM_ELEMS, sizeof(char*),
                              (cmp_func_t)str_cmp);
  check_ordered(sorted_c, NUM_ELEMS, sizeof(char*), (cmp_func_t)str_cmp);
  free(sorted_c);
  free_random_strings(chars, NUM_ELEMS);
}


void test_quicksort(void (*qs)(void *a, size_t n, size_t sz, cmp_func_t cmp)) {
  /* test with ints */
  int *ints = generate_random_ints(NUM_ELEMS);
  (*qs)(ints, NUM_ELEMS, sizeof(int), (cmp_func_t)int_cmp);
  check_ordered(ints, NUM_ELEMS, sizeof(int), (cmp_func_t)int_cmp);
  free_random_ints(ints);
  /* test with chars */
  char **chars = generate_random_strings(NUM_ELEMS);
  (*qs)(chars, NUM_ELEMS, sizeof(char*), (cmp_func_t)str_cmp);
  check_ordered(chars, NUM_ELEMS, sizeof(char*), (cmp_func_t)str_cmp);
  free_random_strings(chars, NUM_ELEMS);
}


void test_selection(void) {
  /* test with ints */
  int *ints = generate_random_ints(NUM_ELEMS);
  int k = random() % NUM_ELEMS; /* select the k'th elem */
  int *e = (int*)selection(ints, NUM_ELEMS, k, sizeof(int), (cmp_func_t)int_cmp);
  assert(ints[k] == *e);
  free_random_ints(ints);
}


#define ITERATIONS 250
int main(void) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    test_mergesort();
    test_quicksort(quicksort);
    test_quicksort(quicksort3);
    test_selection();
  }
  fprintf(stderr, "\n");
  return 0;
}

/* vim: set sw=2 sts=2 : */
