#include <stdlib.h>
#include <string.h>
#include "baas/hashtbl.h"

#define HASHTBL_INIT_BUCKETS 64

static int hashtbl_keycmp(const hash_elem_t *a, const char *key) {
  return strcmp(a->key, key);
}


hashtbl_t * hashtbl_init(free_func_t f) {
  hashtbl_t *h = (hashtbl_t*)malloc(sizeof(hashtbl_t));
  h->free = f;
  h->hash = djb_hash;
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
size_t elf_hash(const unsigned char *key) {
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


size_t pjw_hash(const unsigned char *key) {
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


size_t djb_hash(const unsigned char *key) {
  size_t hash = 5381;
  unsigned char c;
  while ((c = *key++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash & 0x7fffffff; /* clear bitsign just in case */
}

/* vim: set sw=2 sts=2 : */
