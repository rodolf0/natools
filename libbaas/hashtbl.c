#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "baas/hashtbl.h"
#include "baas/vector.h"


struct hashtbl_t {
  int allow_dups;
  size_t size;
  cmp_func_t cmp;
  free_func_t free;
  hash_func_t hash;
  size_t bktnum;
  vector_t **buckets;
};

struct hash_elem_t {
  hashtbl_t *h;
  void *data;
  char key[];
};

#define HASHTBL_INIT_BUCKETS 16

/*******************************************************/
hashtbl_t * hashtbl_init(free_func_t f, cmp_func_t data_cmp) {
  hashtbl_t *h = (hashtbl_t*)zmalloc(sizeof(hashtbl_t));
  h->allow_dups = 0; /* dups not allowed: no easy way to retrieve them */
  h->size = 0;
  h->cmp = data_cmp;
  h->free = f;
  h->hash = sbox_hash;
  h->bktnum = HASHTBL_INIT_BUCKETS;
  h->buckets = (vector_t**)zmalloc(sizeof(vector_t*) * h->bktnum);
  return h;
}

void hashtbl_destroy(hashtbl_t *h) {
  if (!h) return;
  for (size_t i = 0; i < h->bktnum; ++i) {
    if (h->buckets[i]) {
      vector_destroy(h->buckets[i]);
    }
  }
  free(h->buckets);
  free(h);
}

/*******************************************************/
free_func_t hashtbl_set_free(hashtbl_t *h, free_func_t f) {
  free_func_t old = h->free;
  h->free = f;
  return old;
}

cmp_func_t hashtbl_set_cmp(hashtbl_t *h, cmp_func_t c) {
  cmp_func_t old = h->cmp;
  h->cmp = c;
  return old;
}

hash_func_t hashtbl_set_hash(hashtbl_t *h, hash_func_t f) {
  hash_func_t old = h->hash;
  h->hash = f;
  return old;
}

int hashtbl_allow_dups(hashtbl_t *h, int allow) {
  int old = h->allow_dups;
  h->allow_dups = allow;
  return old;
}

/*******************************************************/
size_t hashtbl_size(const hashtbl_t *h) {
  return h ? h->size : 0;
}

size_t hashtbl_numbkt(const hashtbl_t *h) {
  return h ? h->bktnum : 0;
}

/*******************************************************/
void * hash_elem_data(const hash_elem_t *e) {
  return e ? e->data : NULL;
}

const char * hash_elem_key(const hash_elem_t *e) {
  return e ? e->key : NULL;
}

hashtbl_t * hash_elem_tbl(const hash_elem_t *e) {
  return e ? e->h : NULL;
}

/*******************************************************/
static void hashtbl_elem_free(hash_elem_t *e) {
  if (!e) return;
  if (e->h && e->h->free) {
    e->h->free(e->data);
  }
  e->data = NULL;
  free(e);
}

static int hashtbl_keycmp(const hash_elem_t *a, const char *key) {
  return strcmp(a->key, key);
}

static void hashtbl_rehash(hashtbl_t *h) {
  if (!h) return;
  size_t bktnum;
  if (h->size >= h->bktnum)
    bktnum = 2 * h->bktnum;
  else if (h->size > HASHTBL_INIT_BUCKETS && h->size < h->bktnum / 3)
    bktnum = h->bktnum / 2;
  else return;
#ifndef NDEBUG
#ifdef _VERBOSE_
  fprintf(stderr, "hashtbl debug: rehashing... ");
#endif
#endif
  vector_t **buckets = (vector_t**)zmalloc(sizeof(vector_t*) * bktnum);
  for (size_t i = 0; i < h->bktnum; ++i) {
    vector_t *ob = h->buckets[i];
    if (!ob) continue;
    size_t obsz = vector_size(ob);
    for (size_t j = 0; j < obsz; ++j) {
      hash_elem_t *e = (hash_elem_t*)vector_get(ob, j);
      size_t bnum = h->hash(e->key) % bktnum;
      vector_t *nb = buckets[bnum];
      if (!nb)
        nb = vector_init((free_func_t)hashtbl_elem_free, (cmp_func_t)hashtbl_keycmp);
      nb = buckets[bnum] = vector_append(nb, e);
    }
    vector_set_free(ob, NULL);
    vector_destroy(ob);
  }
  free(h->buckets);
  h->buckets = buckets;
#ifndef NDEBUG
#ifdef _VERBOSE_
  fprintf(stderr, " done, oldbkts %zu newbkts %zu\n", h->bktnum, bktnum);
#endif
#endif
  h->bktnum = bktnum;
}

/*******************************************************/
hash_elem_t * hashtbl_insert(hashtbl_t *h, const char *key, void *data) {
  if (!h) return NULL;
  if (!h->hash) {
    fprintf(stderr, "hashtbl insert error: no hash function\n");
    return NULL;
  }
  hashtbl_rehash(h);
  size_t bnum = h->hash(key) % h->bktnum;
  vector_t *b = h->buckets[bnum];
  if (!b) b = vector_init((free_func_t)hashtbl_elem_free, (cmp_func_t)hashtbl_keycmp);
  ssize_t idx;
  hash_elem_t *e = NULL;
  if (!h->allow_dups && (idx = vector_find(b, key)) >= 0) {
    /* no dups: reuse hash_elem replacing data */
    e = (hash_elem_t*)vector_get(b, idx);
    if (h->free) h->free(e->data);
    e->data = data;
  } else {
    /* initialize the new element */
    size_t keylen = strlen(key);
    e = (hash_elem_t*)zmalloc(sizeof(hash_elem_t) + keylen + 1);
    e->h = h;
    memcpy(e->key, key, keylen);
    e->key[keylen] = '\0';
    e->data = data;
    b = h->buckets[bnum] = vector_append(b, e);
    h->size++;
  }
  return e;
}

void hashtbl_remove(hashtbl_t *h, hash_elem_t *e) {
  if (!h || !e) return;
  /* CHECK: might be better to search hashing e->key */
  for (size_t i = 0; i < h->bktnum; ++i) {
    vector_t *b = h->buckets[i];
    if (!b) continue;
    for (size_t j = 0; j < vector_size(b); ++j) {
      if (e == vector_get(b, j)) {
        b = h->buckets[i] = vector_remove(b, j);
        h->size--;
        hashtbl_rehash(h);
        return;
      }
    }
  }
}

/*******************************************************/
void * hashtbl_get(const hashtbl_t *h, const char *key) {
  if (!h) return NULL;
  if (!h->hash) {
    fprintf(stderr, "hashtbl remove error: no hash function\n");
    return NULL;
  }
  size_t bnum = h->hash(key) % h->bktnum;
  vector_t *b = h->buckets[bnum];
  if (!b) return NULL;
  ssize_t idx = vector_find(b, key);
  if (idx >= 0)
    return ((hash_elem_t*)vector_get(b, idx))->data;
  return NULL;
}

size_t hashtbl_delete(hashtbl_t *h, const char *key) {
  if (!h) return 0;
  if (!h->hash) {
    fprintf(stderr, "hashtbl insert error: no hash function\n");
    return 0;
  }
  size_t bnum = h->hash(key) % h->bktnum;
  vector_t *b = h->buckets[bnum];
  if (!b) return 0;
  size_t delcount = 0;
  ssize_t idx = vector_find(b, key);
  while (idx >= 0) {
    b = h->buckets[bnum] = vector_remove(b, idx);
    delcount++;
    idx = vector_find(b, key);
  }
  h->size -= delcount;
  hashtbl_rehash(h);
  return delcount;
}

/*******************************************************/
hash_elem_t * hashtbl_find(const hashtbl_t *h, void *data) {
  if (!h) return NULL;
  if (!h->cmp) {
    fprintf(stderr, "hashtbl find error: no cmp function\n");
    return NULL;
  }
  for (size_t i = 0; i < h->bktnum; ++i) {
    vector_t *b = h->buckets[i];
    if (!b) continue;
    for (size_t j = 0; j < vector_size(b); ++j) {
      if (h->cmp(((hash_elem_t*)vector_get(b, j))->data, data) == 0)
        return (hash_elem_t*)vector_get(b, j);
    }
  }
  return NULL;
}

void hashtbl_foreach(const hashtbl_t *h, void (*f)(void*)) {
  if (!h) return;
  for (size_t i = 0; i < h->bktnum; ++i) {
    vector_t *b = h->buckets[i];
    if (!b) continue;
    for (size_t j = 0; j < vector_size(b); ++j)
      f(((hash_elem_t*)vector_get(b, j))->data);
  }
}

/*******************************************************/
size_t hashtbl_keys(const hashtbl_t *h, char ***keys) {
  if (!h) {
    *keys = NULL;
    return 0;
  }
  *keys = (char**)zmalloc(sizeof(char*) * h->size);
  for (size_t i = 0; i < h->bktnum; ++i) {
    vector_t *b = h->buckets[i];
    if (!b) continue;
    for (size_t j = 0, k = 0; j < vector_size(b); ++j) {
      hash_elem_t *e = (hash_elem_t*)vector_get(b, j);
      size_t keylen = strlen(e->key);
      char *key = (char*)zmalloc(sizeof(char) * (keylen + 1));
      memcpy(key, e->key, keylen);
      key[keylen] = '\0';
      (*keys)[k++] = key;
    }
  }
  return h->size;
}

/*******************************************************/
size_t djb_hash(const char *key) {
  size_t hash = 5381;
  unsigned char c;
  while ((c = (unsigned char)*key++) != '\0')
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

/* http://home.comcast.net/~bretm/hash/10.html */
#define unlikely(x) __builtin_expect(!!(x), 0)
size_t sbox_hash(const char *key) {
  static size_t subst_box[256];
  /* the first time the function is called it will initialize
   * the static substition data and make it available for later use */
  if (unlikely(subst_box[0] == 0)) {
    srandom(1);
    for (size_t hash = 0; hash < 256; hash++)
      subst_box[hash] = random();
  }
  size_t hash = 0;
  while (*key != '\0')
    hash = 3 * (hash ^ subst_box[(unsigned char)*key++]);
  return hash;
}

/* vim: set sw=2 sts=2 : */
