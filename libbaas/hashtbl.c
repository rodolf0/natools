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
  h->nbuckets = HASHTBL_INIT_BUCKETS;
  h->buckets = (vector_t**)malloc(sizeof(vector_t*) * h->nbuckets);
  memset(h->buckets, 0, sizeof(vector_t*) * h->nbuckets);
  return h;
}

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
