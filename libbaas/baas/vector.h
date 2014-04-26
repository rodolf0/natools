#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "common.h"

typedef struct vector_t vector_t;

/* constructor / destructor */
vector_t * vector_init(free_func_t f, cmp_func_t c);
void vector_destroy(vector_t *v);

/* set free/cmp functions. return previous version */
free_func_t vector_set_free(vector_t *v, free_func_t);
cmp_func_t vector_set_cmp(vector_t *v, cmp_func_t);

/* get vector size/capacity */
size_t vector_size(const vector_t *v);
size_t vector_capacity(const vector_t *v);
void ** vector_raw(vector_t *v);

/* resize to len, if bigger zeroed, if smaller freed
 * whole vector might be reallocated: use returned value */
vector_t * vector_resize(vector_t *v, const size_t len);

/* previous value is freed before overwriting */
void vector_set(vector_t *v, ssize_t idx, void *data);
void * vector_get(const vector_t *v, ssize_t idx);

/* inserting at v.size appends, negative idx counts from tail
 * vector might be moved: new vector returned
 * if NULL is returned v is still valid but insert/remove failed */
vector_t * vector_insert(vector_t *v, ssize_t idx, void *data);
vector_t * vector_append(vector_t *v, void *data);
vector_t * vector_remove(vector_t *v, ssize_t idx);

/* retrieve the index where data is located or -1 if not found */
ssize_t vector_find(const vector_t *v, const void *data);

/* execute f for each element of v */
void vector_foreach(const vector_t *v, void (*f)(void*));

#endif /* _VECTOR_H_ */

/* vim: set sw=2 sts=2 : */
