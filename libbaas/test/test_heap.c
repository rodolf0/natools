#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <baas/heap.h>

int int_cmp(const int *a, const int *b) {
  return (*a > *b ? -1 : *b > *a ? 1 : 0);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <list>\n", argv[0]);
    return 1;
  }

  int *data = malloc((argc-1) * sizeof(int));

  /* parse list from cmdline */
  printf("Original list:\n");
  int c;
  heap_t *l = heap_init(NULL, (cmp_func_t)int_cmp);
  for (c = 1; c < argc; c++) {
    data[c-1] = atoi(argv[c]);
    heap_insert(l, &data[c-1]);
    printf("%s ", argv[c]);
  }

  printf("\n\nHeap order:\n");

  int *e;
  while ((e = heap_pop(l))) {
    printf("%d ", *e);
  }
  printf("\n");


  return 0;
}

/* vim: set sw=2 sts=2 : */
