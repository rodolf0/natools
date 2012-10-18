#include <stdlib.h>
#include <string.h>
#include <stdint.h> /* uint32_t */
#include "baas/hashtbl.h"

#define HASHTBL_INIT_BUCKETS 64

static int hashtbl_keycmp(const hash_elem_t *a, const char *key) {
  return strcmp(a->key, key);
}


hashtbl_t * hashtbl_init(free_func_t f) {
  hashtbl_t *h = (hashtbl_t*)malloc(sizeof(hashtbl_t));
  h->free = f;
  h->hash = sbox_hash;
  h->size = 0;
  h->bktnum= HASHTBL_INIT_BUCKETS;
  h->buckets = (vector_t**)malloc(sizeof(vector_t*) * h->bktnum);
  memset(h->buckets, 0, sizeof(vector_t*) * h->bktnum);
  return h;
}


void hashtbl_destroy(hashtbl_t *h) {
  if (!h)
    return;
  size_t i, j;
  for (i = 0; i < h->bktnum; i++)
    if (h->buckets[i]) {
      if (h->free) {
        for (j = 0; j < h->buckets[i]->size; j++) {
          hash_elem_t *e = (hash_elem_t*)vector_get(h->buckets[i], j);
          h->free(e->data);
        }
      }
      vector_destroy(h->buckets[i]);
    }
  free(h->buckets);
  free(h);
}


static void hashtbl_rehash(hashtbl_t *h) {
  if (!h)
    return;
  /*if (h->size > 4 * h->bktnum);*/
  /*if (h->size < h->bktnum / 2);*/
}


hash_elem_t * hashtbl_insert(hashtbl_t *h, char *key, void *data) {
  if (!h)
    return NULL;
  size_t b = h->hash((unsigned char*)key) % h->bktnum;
  if (!h->buckets[b])
    h->buckets[b] = vector_init(free, (cmp_func_t)hashtbl_keycmp);
  /* initialize the new element */
  size_t keylen = strlen(key);
  hash_elem_t *e = (hash_elem_t*)malloc(sizeof(hash_elem_t) + keylen + 1);
  e->key = (char*)e + sizeof(hash_elem_t); /* piggyback the key */
  memcpy(e->key, key, keylen);
  e->key[keylen] = '\0';
  e->data = data;
  /* we could search before insertion to disallow duplicates */
  vector_append(h->buckets[b], e);
  h->size++;
  hashtbl_rehash(h);
  return e;
}


void hashtbl_remove(hashtbl_t *h, char *key) {
  if (!h)
    return;
  size_t b = h->hash((unsigned char*)key) % h->bktnum;
  if (!h->buckets[b])
    return;
  ssize_t idx = vector_find(h->buckets[b], key);
  if (idx < 0)
    return;
  hash_elem_t *e = (hash_elem_t*)vector_get(h->buckets[b], idx);
  if (h->free)
    h->free(e->data);
  vector_remove(h->buckets[b], idx);
  h->size--;
  hashtbl_rehash(h);
}


void * hashtbl_get(hashtbl_t *h, char *key) {
  if (!h)
    return NULL;
  size_t b = h->hash((unsigned char*)key) % h->bktnum;
  if (!h->buckets[b])
    return NULL;
  ssize_t idx = vector_find(h->buckets[b], key);
  if (idx >= 0)
    return ((hash_elem_t*)vector_get(h->buckets[b], idx))->data;
  return NULL;
}


void hashtbl_foreach(hashtbl_t *h, void (*f)(void*)) {
  if (!h)
    return;
  size_t i, j;
  for (i = 0; i < h->bktnum; i++)
    if (h->buckets[i])
      for (j = 0; j < h->buckets[i]->size; j++)
        f(((hash_elem_t*)vector_get(h->buckets[i], j))->data);
}


//TODO: this doesn't work as expected
hash_elem_t * hashtbl_find(hashtbl_t *h, cmp_func_t datacmp, void *data) {
  if (!h)
    return NULL;
  size_t i;
  ssize_t idx;
  for (i = 0; i < h->bktnum; i++)
    if (h->buckets[i] &&
        (idx = vector_find2(h->buckets[i], datacmp, data)) >= 0)
      break;
  if (i < h->bktnum && idx >= 0)
    return vector_get(h->buckets[i], idx);
  return NULL;
}


size_t hashtbl_keys(hashtbl_t *h, char ***keys) {
  if (!h) {
    *keys = NULL;
    return 0;
  }
  *keys = (char**)malloc(sizeof(char*) * h->size);
  size_t i, j, k = 0;
  for (i = 0; i < h->bktnum; i++) {
    if (h->buckets[i])
      for (j = 0; j < h->buckets[i]->size; j++, k++) {
        hash_elem_t *e = (hash_elem_t*)vector_get(h->buckets[i], j);
        size_t keylen = strlen(e->key);
        (*keys)[k] = (char*)malloc(sizeof(char) * (keylen + 1));
        memcpy((*keys)[k], e->key, keylen);
        (*keys)[k][keylen] = '\0';
      }
  }
  return h->size;
}


/* some hash functions */
size_t djb_hash(const unsigned char *key) {
  size_t hash = 5381;
  unsigned char c;
  while ((c = *key++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

/* http://home.comcast.net/~bretm/hash/10.html */
#define unlikely(x) __builtin_expect(!!(x), 0)
size_t sbox_hash(const unsigned char *key) {
  static size_t subst_box[256];
  size_t hash = 0;

  /* the first time the function is called it will initialize
   * the static substition data and make it available for later use */
  if (unlikely(subst_box[0] == 0)) {
    srandom(1);
    for (hash = 0; hash < 256; hash++)
      subst_box[hash] = random();
  }
  while (*key)
    hash = 3 * (hash ^ subst_box[*key++]);
  return hash;
}

/* vim: set sw=2 sts=2 : */
