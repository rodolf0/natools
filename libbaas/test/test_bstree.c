#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../baas/bstree.h"

/* subtree rotations: accesible not in header cause they
 * will unbalance the RB tree. We use them here to test
 * if pointers remain correct inspite of balance alterations */
void bstree_rotate_right(bstree_t *t, bst_node_t *n);
void bstree_rotate_left(bstree_t *t, bst_node_t *n);


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

#ifdef _BALANCED_TREE_
void check_red_node_childs(bst_node_t *n) {
  if (!n)
    return;
  assert(n->color == red || n->color == black);
  if (n->color == red) {
    if (n->l)
      assert(n->l->color == black);
    if (n->r) {
      if (n->r->color != black)
        fprintf(stderr, "Child's color %d\n", n->r->color);
      assert(n->r->color == black);
    }
    if (n->parent)
      assert(n->parent->color == black);
  }
  check_red_node_childs(n->l);
  check_red_node_childs(n->r);
}

int black_height = -1;
int current_height = 0;
void check_black_height(bst_node_t *n) {
  if (!n)
    return;

  assert(n->color == red || n->color == black);

  if (n->color == black)
    current_height++;

  if (!n->l || !n->r) {
    /* reached a leaf */
    /* initialize height if first leaf we reach */
    if (black_height == -1)
      black_height = current_height;
    else
      assert(current_height == black_height);
  }

  check_black_height(n->l);
  check_black_height(n->r);

  /* going back to parents */
  if (n->color == black)
    current_height--;
}
#endif


void generate_test_tree() {
  int n = 0, i, sum = 0, *e=NULL;
  bstree_t *t = bstree_init(free, (cmp_func_t)intcmp);
  bst_node_t *node;

  int inserted = 0, removed = 0, rightrot = 0, leftrot = 0;

  int q = random() % 7000;
  for (i = 0; i < q; i++) {
#ifdef _BALANCED_TREE_
    switch (random() % 2) {
#else
    switch (random() % 4) {
#endif
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
#ifndef _BALANCED_TREE_
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
#endif
    }
#ifdef _BALANCED_TREE_
    /* check RB properties */
    if (t->root)
      assert(t->root->color == black);
    check_red_node_childs(t->root);
    current_height = 0; black_height = -1;
    check_black_height(t->root);
#endif
  }
  fprintf(stderr, "    i: %d, r: %d, rl: %d, rr:%d     ",
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


#define ITERATIONS 100
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
