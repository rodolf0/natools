#include <stdlib.h>
#include <string.h>
#include "baas/hashtbl.h"

#define HASHTBL_INIT_BUCKETS 64

hashtbl_t * hashtbl_init(free_func_t f, cmp_func_t c) {
  hashtbl_t *h = (hashtbl_t*)malloc(sizeof(hashtbl_t));
  h->free = f;
  h->cmp = c;
  h->hash = pjw_hash;
  h->size = 0;
  h->bktnum= HASHTBL_INIT_BUCKETS;
  h->buckets = (vector_t**)malloc(sizeof(vector_t*) * h->bktnum);
  memset(h->buckets, 0, sizeof(vector_t*) * h->bktnum);
  return h;
}

void hashtbl_destroy(hashtbl_t *h) {
  if (!h)
    return;
  size_t i;
  for (i = 0; i < h->bktnum; i++)
    if (h->buckets[i])
      vector_destroy(h->buckets[i]);

}


hash_elem_t * hashtbl_put(hashtbl_t *h, char *key, void *data) {
  if (!h)
    return NULL;
  size_t b = h->hash(key) % h->bktnum;
  if (!h->buckets[b])
    h->buckets[b] = vector_init(h->free, h->cmp);
  /* initialize the new element */
  size_t keylen = strlen(key);
  hash_elem_t *e = (hash_elem_t*)malloc(sizeof(hash_elem_t) + keylen + 1);
  e->key = (char*)e + sizeof(hash_elem_t); /* piggyback the key */
  memcpy(e->key, key, keylen);
  e->key[keylen] = '\0';
  e->data = data;
  /* insert the new element into the hashtable */
  vector_insert(h->buckets[b], -1, e);
  return e;
}


void hashtbl_remove(hashtbl_t *h, char *key) {
  if (!h)
    return;
  size_t b = h->hash(key) % h->bktnum;
  if (!h->buckets[b])
    return;
  ssize_t idx = vector_find(h->buckets[b], key);
  if (idx >= 0)
    vector_remove(h->buckets[b], idx);
}


void * hashtbl_get(hashtbl_t *h, char *key) {
  if (!h)
    return NULL;
  size_t b = h->hash(key) % h->bktnum;
  if (!h->buckets[b])
    return NULL;
  ssize_t idx = vector_find(h->buckets[b], key);
  if (idx >= 0)
    return vector_get(h->buckets[b], idx);
  return NULL;
}


void hashtbl_foreach(hashtbl_t *h, void (*f)(void*)) {
  if (!h)
    return;
  size_t i;
  for (i = 0; i < h->bktnum; i++)
    vector_foreach(h->buckets[i], f);
}


hash_elem_t * hashtbl_find(hashtbl_t *h, void *data) {
  if (!h)
    return NULL;
  size_t i;
  ssize_t idx;
  for (i = 0; i < h->bktnum; i++)
    if (h->buckets[i] && (idx = vector_find(h->buckets[i], data)) >= 0)
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
        *keys[k] = (char*)malloc(sizeof(char) * (keylen + 1));
        memcpy(*keys[k], e->key, keylen);
        *keys[k][keylen] = '\0';
      }
  }
  return h->size;
}


/* some hash functions */
size_t elf_hash(const char *key) {
  size_t hash = 0, test = 0;
  unsigned char c;
  while ((c = *key++)) {
    hash = (hash << 4) + c;
    if ((test = hash & 0xf0000000) != 0)
      hash ^= (test >> 24);
    hash &= ~test;
  }
  return hash & 0x7fffffff; /* clear bitsign just in case */
}


size_t pjw_hash(const char *key) {
  const size_t szbits = sizeof(size_t) * 8;
  const size_t threeq = (szbits * 3) / 4;
  const size_t aeigth = szbits / 8;
  const size_t hibits = 0xffffffff << (szbits - aeigth);

  size_t hash = 0, test = 0;
  unsigned char c;
  while ((c = *key++)) {
    hash = (hash << aeigth) + c;
    if ((test = hash & hibits) != 0)
      hash = (hash ^ (test >> threeq)) & (~hibits);
  }
  return hash & 0x7fffffff; /* clear bitsign just in case */
}


size_t djb_hash(const char *key) {
  size_t hash = 5381;
  unsigned char c;
  while ((c = *key++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash & 0x7fffffff; /* clear bitsign just in case */
}

/* vim: set sw=2 sts=2 : */
