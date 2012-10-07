#ifndef _HASHTBL_H_
#define _HASHTBL_H_

#include "common.h"
#include "vector.h"

typedef struct _hashtbl_t {
  size_t nbuckets;
  vector_t **buckets;
  size_t size;
  cmp_func_t cmp;
  free_func_t free;
  hash_func_t hash;
} hashtbl_t;

typedef struct _hash_elem_t {
  char *key;
  void *data;
} hash_elem_t;


/* constructor / destructor */
hashtbl_t * hashtbl_init(free_func_t f, cmp_func_t c, hash_func_t h);
void hashtbl_destroy(hashtbl_t *h);

int hashtbl_put(hashtbl_t *h, char *key, void *data);
void hashtbl_remove(hashtbl_t *h, char *key);
void * hashtbl_get(hashtbl_t *h, char *key);

void hashtbl_foreach(hashtbl_t *h, void (*f)(void*));
char * hashtbl_find(hashtbl_t *h, void *data);

/* return the keys stored in the hashtable */
size_t hashtbl_keys(hashtbl_t *h, char **keys);

/* some hash functions */
size_t elf_hash(const unsigned char *key);
size_t pjw_hash(const unsigned char *key);
size_t djb_hash(const unsigned char *key);
/* http://www.partow.net/programming/hashfunctions/index.html */

#endif /* _HASHTBL_H_ */

/* vim: set sw=2 sts=2 : */
