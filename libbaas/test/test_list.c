#include <stdio.h>
#include <string.h>
#include "../list.h"
#include "../heap.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <list>\n", argv[0]);
    return 1;
  }

  /* parse list from cmdline */
  int c;
  list_t *l = list_init(NULL, (int(*)(const void*, const void*))strcmp);
  for (c = 0; c < argc-1; c++)
    list_queue(l, argv[c+1]);

  printf("Original list\n");
  /* print original list */
  list_node_t *n = l->first;
  while (n) {
    printf("%s ", (char*)n->data);
    n = n->next;
  }
  printf("\n\n");

  l = list_mergesort(l);

  printf("String Sorted List\n");
  /* print sorted list */
  n = l->first;
  while (n) {
    printf("%s ", (char*)n->data);
    n = n->next;
  }
  printf("\n");

  list_destroy(l);
  return 0;
}

/* vim: set sw=2 sts=2 : */
