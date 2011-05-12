#ifndef _BSTREE_H_
#define _BSTREE_H_

#include "common.h"

typedef struct _bst_node {
  bst_node_t *parent;
  bst_node_t *r;
  bst_node_t *l;
  void *data;
} bst_node_t;

typedef struct _bstree {
  bst_node_t *root;

  cmp_func_t cmp;
  free_func_t free;
} bstree_t;

bstree_t * bstree_init(free_func_t f, cmp_func_t c);
void bstree_destroy(bstree_t *t);

bst_node_t * bstree_insert(bstree_t *t, void *data);
void bstree_delete(bstree_t *t, void *data);
bst_node_t * bstree_find(bstree_t *t, void *data);

#endif /* _BSTREE_H_ */

/* vim: set sw=2 sts=2 : */
