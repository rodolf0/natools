#ifndef _BSTREE_H_
#define _BSTREE_H_

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

bstree_t * bstree_init();
void bstree_destroy(bstree_t *t);

#endif /* _BSTREE_H_ */

/* vim: set sw=2 sts=2 : */
