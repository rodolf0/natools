#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../baas/bstree.h"

int intcmp(const int *a, const int *b) {
  return *a > *b ? 1 : (*a < *b ? -1 : 0);
}

int totalsum;
void acumulate(int *x) {
  totalsum += *x;
}

int minimum;
void check_set_min(int *x) {
  assert(minimum <= *x);
  minimum = *x;
}


void generate_test_tree() {
  int n = 0, i, sum = 0, *e=NULL;
  bstree_t *t = bstree_init(free, (cmp_func_t)intcmp);
  bst_node_t *node;

  int inserted = 0, removed = 0, rightrot = 0, leftrot = 0;

  for (i = 0; i < 10000; i++) {
    switch (random() % 4) {
      case 0:
        e = malloc(sizeof(int)); *e = random() % 512;
        n++; sum += *e;
        bstree_insert(t, e);
        inserted++;
        break;
      case 1:
        /* try n find a possibly inserted number to remove it */
        e = malloc(sizeof(int)); *e = random() % 512;
        node = bstree_find(t, e);
        if (node) {
          n--; sum -= *(int*)node->data;
          bstree_remove(t, node);
          removed++;
        }
        free(e);
        break;
      case 2:
        /* try n find a possibly inserted number to rotate it's node */
        e = malloc(sizeof(int)); *e = random() % 512;
        node = bstree_find(t, e);
        if (node) {
          bstree_rotate_left(t, node);
          leftrot++;
        }
        free(e);
        break;
      case 3:
        /* try n find a possibly inserted number to rotate it's node */
        e = malloc(sizeof(int)); *e = random() % 512;
        node = bstree_find(t, e);
        if (node) {
          bstree_rotate_right(t, node);
          rightrot++;
        }
        free(e);
        break;
    }
  }
  fprintf(stderr, "    i: %d, r: %d, rl: %d, rr:%d",
          inserted, removed, leftrot, rightrot);

  /* check all elements are correct */
  assert(t->size == n);
  totalsum = 0;
  bstree_foreach(t, (void (*)(void*))acumulate, inorder);
  assert(totalsum == sum);

  /* check propper sorting of elements*/
  minimum = 0;
  bstree_foreach(t, (void (*)(void*))check_set_min, inorder);

  bstree_destroy(t);
}

#define ITERATIONS 1000
int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    generate_test_tree();
  }
  fprintf(stderr, "\n");
  return 0;
}

/* vim: set sw=2 sts=2 : */
