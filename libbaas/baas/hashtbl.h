#ifndef _HASHTBL_H_
#define _HASHTBL_H_

#include "common.h"
#include "vector.h"

typedef struct _hashtbl_t {
  size_t bktnum;
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
hashtbl_t * hashtbl_init(free_func_t f);
void hashtbl_destroy(hashtbl_t *h);

hash_elem_t * hashtbl_insert(hashtbl_t *h, char *key, void *data);
// TOOD: sync with bstree API, make remove take a hash_elem_t
void hashtbl_remove(hashtbl_t *h, char *key);
void * hashtbl_get(hashtbl_t *h, char *key);

void hashtbl_foreach(hashtbl_t *h, void (*f)(void*));
hash_elem_t * hashtbl_find(hashtbl_t *h, cmp_func_t datacmp, void *data);

/* return the keys (and number of) stored in the hashtable
 * if return is greater than 0 the user needs to free the keys */
size_t hashtbl_keys(hashtbl_t *h, char ***keys);

/* some hash functions (http://www.strchr.com/hash_functions) */
size_t djb_hash(const unsigned char *key);
size_t meiyan_hash(const unsigned char *key);

#endif /* _HASHTBL_H_ */

/* vim: set sw=2 sts=2 : */
