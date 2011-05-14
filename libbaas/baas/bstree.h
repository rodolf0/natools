#ifndef _BSTREE_H_
#define _BSTREE_H_

#include "common.h"

typedef enum {
  leftheavy,
  balanced,
  rightheavy
} bst_balance_t;

typedef struct _bst_node {
  bst_node_t *parent;
  bst_node_t *r;
  bst_node_t *l;
  void *data;
  bst_balance_t balance;
} bst_node_t;

typedef struct _bstree {
  bst_node_t *root;
  size_t size;
  cmp_func_t cmp;
  free_func_t free;
} bstree_t;

/* constructor / destructor */
bstree_t * bstree_init(free_func_t f, cmp_func_t c);
void bstree_destroy(bstree_t *t);

/* remove a node (previously find with bstree_find */
void bstree_remove(bstree_t *t, bst_node_t *n);
bst_node_t * bstree_insert(bstree_t *t, void *data);
/* inset a child into a subtree, mainly used by bstree_insert internally */
bst_node_t * bstree_create_child(bstree_t *t, bst_node_t *b, void *data);

bst_node_t * bstree_find(bstree_t *t, void *data);
void bstree_foreach(bstree *t, void (*f)(void *));

/* subtree rotations */
void bstree_rotate_left(bstree_t *t, bst_node_t *n);
void bstree_rotate_right(bstree_t *t, bst_node_t *n);

#endif /* _BSTREE_H_ */

/* vim: set sw=2 sts=2 : */
