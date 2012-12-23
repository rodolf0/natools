#include <stdlib.h>
#include "baas/bstree.h"

#ifdef _DEBUG_
#include <assert.h>
#endif


/* functions for internal use only, not static to allow testing n hacking */
/* destroy whole descendence of n */
void bstree_destroy_childs(bst_node_t *n, free_func_t nfree);
/* inset a child into a subtree, mainly used by bstree_insert internally */
bst_node_t * bstree_create_child(bstree_t *t, bst_node_t *b, void *data);
/* subtree rotations: for internal use only, else will unbalance RB tree */
void bstree_rotate_right(bstree_t *t, bst_node_t *n);
void bstree_rotate_left(bstree_t *t, bst_node_t *n);
/* iterate on a subtree */
void bsstree_foreach(bst_node_t *t, void (*f)(void *), bst_traversal_t order);
#ifdef _BALANCED_TREE_
/* balance tree on insertion */
static void red_black_rebalance_insert(bstree_t *t, bst_node_t *n);
static void red_black_rebalance_delete(bstree_t *t, bst_node_t *n);
static bst_node_t * bstree_fix_double_red(bstree_t *t, bst_node_t *n);
static void bstree_fix_negative_black(bstree_t *t, bst_node_t *n);
#endif


bstree_t * bstree_init(free_func_t tfree, cmp_func_t cmp, int allow_dups) {
  if (!cmp)
    return NULL;
  bstree_t *t = malloc(sizeof(bstree_t));
  t->free = tfree;
  t->cmp = cmp;
  t->size = 0;
  t->root = NULL;
  t->allow_dups = allow_dups;
  return t;
}

void bstree_destroy(bstree_t *t) {
  if (!t)
    return;
  if (t->free)
    bsstree_foreach(t->root, t->free, inorder);
  bstree_destroy_childs(t->root, free);
  free(t);
}

/* can't use bstree_foreach cause it acts directly on data */
void bstree_destroy_childs(bst_node_t *n, free_func_t nfree) {
  if (!n || !nfree)
    return;
  bstree_destroy_childs(n->l, nfree);
  bstree_destroy_childs(n->r, nfree);
  nfree(n);
}


/* find the slot where insertion should happen, then delegate to create_child */
bst_node_t * bstree_insert(bstree_t *t, void *data) {
  if (!t)
    return NULL;
  bst_node_t *cur;
  for (cur = t->root; cur;) {
    /* this implies that less or equal precede a node */
    int cmp = t->cmp(data, cur->data);
    if (cmp > 0) {
      if (!cur->r)
        break;
      cur = cur->r;
    } else if (cmp < 0 || t->allow_dups) {
      if (!cur->l)
        break;
      cur = cur->l;
    } else { /* element present and no dups allowed */
      if (t->free)
        t->free(cur->data);
      cur->data = data;
      return cur;
    }
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
  new->data = data;

  if (!parent) {
    t->root = new;
  } else {
    new->parent = parent;
    if (cmp > 0)
      parent->r = new;
    else
      parent->l = new;
  }

#ifdef _BALANCED_TREE_
  new->color = red; /* choose red so that black height is not affected */
  red_black_rebalance_insert(t, new);
#endif

  t->size++;
  return new;
}


#ifdef _BALANCED_TREE_
/* get a node's grand parent */
static inline bst_node_t * grandparent(bst_node_t *n) {
  if (!n || !n->parent)
    return NULL;
  return n->parent->parent;
}

/* get a node's uncle: parent's sibiling (mainly used to check color) */
static inline bst_node_t * uncle(bst_node_t *n) {
  bst_node_t *g = grandparent(n);
  if (!g)
    return NULL;
  if (n->parent == g->l)
    return g->r;
  else
    return g->l;
}

/* get a node's sibling */
static inline bst_node_t * sibling(bst_node_t *n) {
  if (!n || !n->parent)
    return NULL;
  if (n == n->parent->l)
    return n->parent->r;
  else //if (n == n->parent->r)
    return n->parent->l;
}


/* fix two red nodes in a row, return the one that ends in n's place */
static bst_node_t * bstree_fix_double_red(bstree_t *t, bst_node_t *n) {
  if (!t || !n || !n->parent)
    return NULL;

#ifdef _DEBUG_
  assert(n->color == red && n->parent->color == red);
#endif
  if (n->color != red || n->parent->color != red)
    return NULL;

  bst_node_t *g = grandparent(n);
#ifdef _DEBUG_
  assert(g->color == black || g->color == double_black);
#endif
  /* if red nodes are in zigzag, then align them */
  if (n == n->parent->r && n->parent == g->l) {
    bstree_rotate_left(t, n->parent);
    n = n->l;
  } else if (n == n->parent->l && n->parent == g->r) {
    bstree_rotate_right(t, n->parent);
    n = n->r; /* keep working on the leafs */
  }

  /* adjust aligned double red */
  if (n == n->parent->l && n->parent == g->l) {
    bstree_rotate_right(t, g);
  } else if (n == n->parent->r && n->parent == g->r) {
    bstree_rotate_left(t, g);
  }

#ifdef _DEBUG_
  assert(g->color == black || g->color == double_black);
#endif
  if (g->color == black) {
    n->color = black;
    n->parent->color = red;
  } else if (g->color == double_black) {
    n->color = black;
    n->parent->color = black;
    g->color = black; // back from double_black
  }

  /* return the node in n's place after all rotations */
  return n;
}

static inline bst_node_t * bstree_check_two_reds(bstree_t *t, bst_node_t *n) {
  if (!t || !n || n->color != red)
    return NULL;
  if (n->l && n->l->color == red)
    return n->l;
  else if (n->r && n->r->color == red)
    return n->r;
  return NULL;
}

/* fix negative_black (result of bubbling), return  node in n's place */
static void bstree_fix_negative_black(bstree_t *t, bst_node_t *n) {
  if (!t || !n || !n->parent)
    return;
#ifdef _DEBUG_
  assert(n->color == negative_black && n->parent->color == double_black);
#endif

  n->color = black;
  n->parent->color = black;

  if (n == n->parent->l) {
    n->r->color = red;
    bstree_rotate_right(t, n->parent);
    if ((n = bstree_check_two_reds(t, n->r->l)))
      bstree_fix_double_red(t, n);
  } else {
    n->l->color = red;
    bstree_rotate_left(t, n->parent);
    if ((n = bstree_check_two_reds(t, n->l->r)))
      bstree_fix_double_red(t, n);
  }
}

static void bstree_bubble_up(bstree_t *t, bst_node_t *n) {
  if (!t || !n)
    return;

#ifdef _DEBUG_
  assert(n->color == double_black || n->color == black);
#endif

  if (n->color == double_black)
    n->color = black;
  else if (n->color == black)
    n->color = red;

  if (n->parent) {
    if (n->parent->color == black)
      n->parent->color = double_black;
    else if (n->parent->color == red)
      n->parent->color = black;

    bst_node_t *s = sibling(n);
    if (s && s->color == black) {
      s->color = red;
    } else if (s && s->color == red) {
      s->color = negative_black;
    }

    bst_node_t *xs;
    if (s && s->color == negative_black) {
#ifdef _DEBUG_
      assert(s->parent->color == double_black);
#endif
      bstree_fix_negative_black(t, s);
    } else if (s && s->color == red) {
      if ((xs = bstree_check_two_reds(t, s)))
        bstree_fix_double_red(t, xs);
    }

    if (n->parent->color == double_black)
      bstree_bubble_up(t, n->parent);
  }
}

/* http://matt.might.net/articles/red-black-delete/ */
static void red_black_rebalance_insert(bstree_t *t, bst_node_t *n) {
  if (!t || !n)
    return;
  bst_node_t *u, *g, *s;

  if (!n->parent) /* the root is always black */
    n->color = black;
  else {
    if (n->parent->color == red) {
      /* RB property violated: double red's */
      if ((u = uncle(n)) && u->color == red) {
        /* flip n's parent and uncle to black to solve violation */
        n->parent->color = black;
        u->color = black;
        /* we need to keep black height so flip grandparent to red */
        g = grandparent(n); g->color = red;
        /* since g's parent might also be red, rebalance on g */
        red_black_rebalance_insert(t, g);
      } else {
        /* no uncle or ain't red: need to rotate */
        n = bstree_fix_double_red(t, n);
        s = sibling(n);
        /* flip colors */
        n->parent->color = black;
        n->color = red; s->color = red;
      }
    }
  }
}

/* we assume that n has at most one child. */
static void red_black_rebalance_delete(bstree_t *t, bst_node_t *n) {
  if (!t || !n || (n->l && n->r))
    return;

  if (n->color == red)
    return;
  else if (n->l) {
    /* if node is black and has only one child, then child is red, recolor */
    n->l->color = black;
  } else if (n->r) {
    n->r->color = black;
  } else {
    /* we're dealing with a leaf node and it's black...
     * fact: a black leaf always has a sibling */
    if (!n->parent) return; /* it's the root */
    bstree_bubble_up(t, n);
  }
}
#endif


/* handle deletion of a node from a tree */
void bstree_remove(bstree_t *t, bst_node_t *n) {
  if (!t || !n)
    return;

  /* a node with no children: just remove link from parent */
  if (!n->l && !n->r) {
#ifdef _BALANCED_TREE_
  red_black_rebalance_delete(t, n);
#endif
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

    void *xchgdata = successor->data;
    successor->data = n->data;
    n->data = xchgdata;
    return bstree_remove(t, successor);

  } else /* n->l ^ n->r */ {
    /* a node with just one children*/
#ifdef _BALANCED_TREE_
  red_black_rebalance_delete(t, n);
#endif
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


/* make node n the right child of n's left child
 * http://en.wikipedia.org/wiki/Tree_rotation */
void bstree_rotate_right(bstree_t *t, bst_node_t *n) {
  if (!t || !n || !n->l) /* can't rotate right if no n->l */
    return;
  bst_node_t *left = n->l;
  /* link n's parent to left (new subroot) */
  if (n->parent) {
    if (n == n->parent->l)
      n->parent->l = left;
    if (n == n->parent->r)
      n->parent->r = left;
  } else
    t->root = left;
  /* update left to point to new parent */
  left->parent = n->parent;
  /* update n to point to new parent (left) */
  n->parent = left;
  /* if left has a right child, it'll become n's left */
  n->l = left->r;
  if (left->r)
    left->r->parent = n;
  /* finally link n as left's new right son */
  left->r = n;
}


/* make node n the left child of n's right child */
void bstree_rotate_left(bstree_t *t, bst_node_t *n) {
  if (!t || !n || !n->r) /* can't rotate left if no n->r */
    return;
  bst_node_t *right= n->r;
  /* link n's parent to right (new subroot) */
  if (n->parent) {
    if (n == n->parent->l)
      n->parent->l = right;
    if (n == n->parent->r)
      n->parent->r = right;
  } else
    t->root = right;
  /* update right to point to new parent */
  right->parent = n->parent;
  /* update n to point to new parent (right) */
  n->parent = right;
  /* if right has a left child, it'll become n's right */
  n->r = right->l;
  if (right->l)
    right->l->parent = n;
  /* finally link n as right's new left son */
  right->l = n;
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


/* call f on each node */
void bstree_foreach(bstree_t *t, void (*f)(void *), bst_traversal_t order) {
  if (!t || !f)
    return;
  bsstree_foreach(t->root, f, order);
}

void bsstree_foreach(bst_node_t *n, void (*f)(void *), bst_traversal_t order) {
  if (!n || !f)
    return;
  switch (order) {
    case preorder:
      f(n->data); // calls the function on itself, then on children
      bsstree_foreach(n->l, f, order);
      bsstree_foreach(n->r, f, order);
      break;
    case inorder:
      bsstree_foreach(n->l, f, order);
      f(n->data);
      bsstree_foreach(n->r, f, order);
      break;
    case postorder:
      bsstree_foreach(n->l, f, order);
      bsstree_foreach(n->r, f, order);
      f(n->data); // call function on children, then itself
      break;
    case breadthfirst:
      //
      //TODO
      //
      break;
  }
}

/* vim: set sw=2 sts=2 : */
