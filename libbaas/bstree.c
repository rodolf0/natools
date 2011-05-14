#include "baas/bstree.h"

/* http://eternallyconfuzzled.com/tuts/datastructures/jsw_tut_bst1.aspx */

bstree_t * bstree_init(free_func_t tfree, cmp_func_t cmp) {
  if (!cmp)
    return NULL;
  bstree_t *t = malloc(sizeof(bstree_t));
  t->free = tfree;
  t->cmp = cmp;
  t->size = 0;
  t->root = NULL;
  return t;
}

void bstree_destroy(bstree_t *t) {
  // TODO: free all nodes

  free(t);
}


/* find the slot where insertion should happen, then delegate to create_child */
bst_node_t * bstree_insert(bstree_t *t, void *data) {
  if (!t)
    return NULL;
  bst_node_t *cur;
  for (cur = t->root; cur;) {
    /* this implies that less or equal precede a node */
    if (t->cmp(data, cur->data) > 0) {
      if (!cur->r)
        break;
      cur = cur->r;
    } else {
      if (!cur->l)
        break;
      cur = cur->l;
    }
    /* testing for == and returning if found avoids duplicat data */
  }
  return bstree_create_child(t, cur, data);
}


bst_node_t * bstree_create_child(bstree_t *t, bst_node_t *parent, void *data) {
  if (!t)
    return NULL;

  int cmp = 0;
  if (parent) {
    cmp = t->cmp(data, parent->data);
    /* just some checks in case this is used by humans */
    if (cmp > 0 && parent->r)
      return NULL;  /* node already has data, don't allow overwrite */
    if (cmp <= 0 && parent->l)
      return NULL;  /* node already has data, don't allow overwrite */
  } else if (t->root)
    return NULL; /* can't insert root on a rooted tree */

  bst_node_t *new = malloc(sizeof(bst_node_t));
  new->parent = new->r = new->l = NULL;
  new->balance = balanced;
  new->data = data;

  if (!parent) {
    t->root = new;
  } else {
    new->parent = parent;
    if (cmp > 0) {
      parent->r = new;
      if (parent->l)
        parent->balance = balanced;
      else
        parent->balance = rightheavy;
    } else {
      parent->l = new;
      if (parent->r)
        parent->balance = balanced;
      else
        parent->balance = leftheavy;
    }
    // TODO: tree rebalance here: self balancing tree
  }

  t->size++;
  return new;
}


/* handle deletion of a node from a tree */
void bstree_remove(bstree_t *t, bst_node_t *n) {
  if (!t || !n)
    return;

  /* a node with no children: just remove link from parent */
  if (!n->l && !n->r) {
    if (n->parent) {
      if (n == n->parent->l)
        n->parent->l = NULL;
      if (n == n->parent->r)
        n->parent->r = NULL;
    } else
      t->root = NULL;

  } else if (n->l && n->r) {
    /* a node with both children: find inorder successor
     * (predeccessor works too) and move that one here */
    bst_node_t *successor = n->r;
    while (successor->l)
      successor = successor->l;

    /* unlink suc's parent from it, relink it to suc's child if available */
    if (successor != n->r)
      successor->parent->l = successor->r; // NULL is correct if !suc->r
    /* unlink suc's child from it, relink it to suc's parent */
    if (successor != n->r && successor->r)
      successor->r->parent = successor->parent; // else same parent

    /* link succesor new surroundings (what was n's before) */
    successor->parent = n->parent;
    successor->l = n->l;
    if (successor != n->r)
      successor->r = n->r; // else same right child

    /* relink n's surroundings to its successor */
    if (n->parent) {
      if (n == n->parent->l)
        n->parent->l = successor;
      if (n == n->parent->r)
        n->parent->r = successor;
    } else
      t->root = successor;
    n->l->parent = successor;
    if (successor != n->r)
      n->r->parent = successor; // else covered by  suc->parent = n->parent

  } else /* n->l ^ n->r */ {
    /* a node with just one children*/
    if (n->r) {
      if (n->parent) {
        if (n == n->parent->l)
          n->parent->l = n->r;
        if (n == n->parent->r)
          n->parent->r = n->r;
      } else
        t->root = n->r;
      n->r->parent = n->parent;
    } else /* n->l */ {
      if (n->parent) {
        if (n == n->parent->l)
          n->parent->l = n->l;
        if (n == n->parent->r)
          n->parent->r = n->l;
      } else
        t->root = n->l;
      n->l->parent = n->parent;
    }
  }

  /* free data and node */
  if (t->free)
    t->free(n->data);
  free(n);
  t->size--;
}


bst_node_t * bstree_find(bstree_t *t, void *data) {
  if (!t)
    return NULL;
  bst_node_t *cur = t->root;
  while (cur) {
    int cmp = t->cmp(data, cur->data);
    if (cmp > 0)
      cur = cur->r;
    else if (cmp < 0)
      cur = cur->l;
    else
      break;
  }
  return cur;
}


/* vim: set sw=2 sts=2 : */
