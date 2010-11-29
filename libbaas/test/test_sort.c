#include <stdio.h>
#include <stdlib.h>
#include <baas/sort.h>
#include <baas/sort-cmp.h>

void print_array_s(char **a, int n) {
  int i;

  for (i = 0; i < n; i++)
    printf("%s ", a[i]);
  printf("\n");
}

void print_array(int *a, int n) {
  int i;

  for (i = 0; i < n; i++)
    printf("%d ", a[i]);
  printf("\n");
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "%s: <algorithm> <input list>\n", argv[0]);
    return 1;
  }

  int i, n = argc-2;
  int *ints = malloc(n * sizeof(int));
  char **chars = malloc(n * sizeof(char*));
  char *algorithm = argv[1];

  /* parse input list */
  for (i = 0; i < n; i++) {
    ints[i] = atoi(argv[i+2]);
    chars[i] = argv[i+2];
  }

  if (strcmp(algorithm, "mergesort") == 0) {
    int *ri = mergesort(ints, n, sizeof(int), (cmp_func_t)int_cmp);
    char **rc = mergesort(chars, n, sizeof(char*), (cmp_func_t)str_cmp);
    print_array(ri, n);
    print_array_s(rc, n);
    free(ri);
    free(rc);
  } else if (strcmp(algorithm, "quicksort") == 0) {
    quicksort(ints, n, sizeof(int), (cmp_func_t)int_cmp);
    quicksort(chars, n, sizeof(char*), (cmp_func_t)str_cmp);
    print_array(ints, n);
    print_array_s(chars, n);
  } else if (strcmp(algorithm, "quicksort3") == 0) {
    quicksort3(ints, n, sizeof(int), (cmp_func_t)int_cmp);
    quicksort3(chars, n, sizeof(char*), (cmp_func_t)str_cmp);
    print_array(ints, n);
    print_array_s(chars, n);
  } else if (strcmp(algorithm, "partition") == 0) {
    size_t kth = ints[0];
    size_t pi = partition(ints+1, n-1, kth, sizeof(int), (cmp_func_t)int_cmp);
    size_t pc = partition2(chars+1, n-1, kth, sizeof(char*), (cmp_func_t)str_cmp);
    print_array(ints+1, n-1);
    print_array_s(chars+1, n-1);
    printf("piv-ints: %lu, piv-chars: %lu\n", pi, pc);
  } else if (strcmp(algorithm, "partition3") == 0) {
    size_t kth = ints[0];
    size_t m, g;
    partition3(ints+1, n-1, kth, &m, &g, sizeof(int), (cmp_func_t)int_cmp);
    printf("eq-ints: %lu, gt-ints: %lu\n", m, g);
    partition32(chars+1, n-1, kth, &m, &g, sizeof(char*), (cmp_func_t)str_cmp);
    printf("eq-chars: %lu, gt-chars: %lu\n", m, g);
    print_array(ints+1, n-1);
    print_array_s(chars+1, n-1);
  } else if (strcmp(algorithm, "selection") == 0) {
    size_t kth = ints[0];
    int *pi = selection(ints+1, n-1, kth, sizeof(int), (cmp_func_t)int_cmp);
    char **pc = selection(chars+1, n-1, kth, sizeof(char*), (cmp_func_t)str_cmp);
    printf("kth-ints: %d, kth-chars: %s\n", *pi, *pc);
  }

  /* free resources */
  free(ints);
  free(chars);
  return 0;
}

/* vim: set sw=2 sts=2 : */
