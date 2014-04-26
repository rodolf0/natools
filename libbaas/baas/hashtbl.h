#ifndef _HASHTBL_H_
#define _HASHTBL_H_

#include "common.h"

typedef struct hashtbl_t hashtbl_t;
typedef struct hash_elem_t hash_elem_t;

/* constructor / destructor */
hashtbl_t * hashtbl_init(free_func_t f, cmp_func_t data_cmp);
void hashtbl_destroy(hashtbl_t *h);

/* set free/cmp/hash functions. return previous version */
free_func_t hashtbl_set_free(hashtbl_t *h, free_func_t);
cmp_func_t hashtbl_set_cmp(hashtbl_t *h, cmp_func_t);
hash_func_t hashtbl_set_hash(hashtbl_t *h, hash_func_t);
int hashtbl_allow_dups(hashtbl_t *h, int allow);

/* get hashtbl size and number of buckets */
size_t hashtbl_size(const hashtbl_t *h);
size_t hashtbl_numbkt(const hashtbl_t *h);

/* get hash_elem info */
void * hash_elem_data(const hash_elem_t *e);
const char * hash_elem_key(const hash_elem_t *e);
hashtbl_t * hash_elem_tbl(const hash_elem_t *e);

/* insert data or replace existent (unordered set/dict) */
hash_elem_t * hashtbl_insert(hashtbl_t *h, const char *key, void *data);
/* remove and free element */
void hashtbl_remove(hashtbl_t *h, hash_elem_t *e);
/* get first element matching key, if dups allowed can't retrieve next */
void * hashtbl_get(const hashtbl_t *h, const char *key);
/* delete elements matching the key, return deleted count */
size_t hashtbl_delete(hashtbl_t *h, const char *key);
#if 0
/* get all returns a count and an array of elements matching the key
 * if return is greater than 0 the user should free datas */
size_t hashtbl_get_all(const hashtbl_t *h, const char *key, void **datas);
#endif

/* retrieve the index where data is located or -1 if not found */
hash_elem_t * hashtbl_find(const hashtbl_t *h, void *data);
/* execute f for each element of h */
void hashtbl_foreach(const hashtbl_t *h, void (*f)(void*));

/* return the keys (and number) stored in the hashtable
 * if return is greater than 0 the user needs to free the keys */
size_t hashtbl_keys(const hashtbl_t *h, char ***keys);

/* some hash functions (http://www.strchr.com/hash_functions) */
size_t djb_hash(const char *key);
size_t sbox_hash(const char *key);

#endif /* _HASHTBL_H_ */

/* vim: set sw=2 sts=2 : */
