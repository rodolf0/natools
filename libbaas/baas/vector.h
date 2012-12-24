#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "common.h"

typedef struct _vector_t {
  size_t size;
  size_t cap;
  void **data;
  free_func_t free;
  cmp_func_t cmp;
} vector_t;

/* constructor / destructor */
vector_t * vector_init(free_func_t f, cmp_func_t c);
void vector_destroy(vector_t *v);

/* inserting at v->size (one past the last) is allowed (append)
 * negative index counts backwards (eg: -1 -> last)
 * returns positive insertion idx on success, negative on failure */
ssize_t vector_append(vector_t *v, void *data);
ssize_t vector_insert(vector_t *v, ssize_t idx, void *data);
void vector_remove(vector_t *v, ssize_t idx);

void * vector_get(vector_t *v, ssize_t idx);
/* if there's a previous value at the index it will be freed */
void vector_set(vector_t *v, ssize_t idx, void *data);

/* resize the vector to have [len] size
 * if new size is bigger new elements will be null
 * if smaller elements will be freed */
void vector_resize(vector_t *v, const size_t len);

/* retrieve the index where data is located or -1 */
ssize_t vector_find(vector_t *v, void *data);

void vector_foreach(vector_t *v, void (*f)(void*));

#endif /* _VECTOR_H_ */

/* vim: set sw=2 sts=2 : */
