#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "baas/vector.h"

struct vector_t {
  size_t size;
  size_t cap;
  free_func_t free;
  cmp_func_t cmp;
  void *data[];
};

#define VECTOR_INIT_CAPACITY 4

/*******************************************************/
vector_t * vector_init(free_func_t f, cmp_func_t c) {
  vector_t *v = (vector_t*)zmalloc(sizeof(vector_t) +
                                   sizeof(void*) * VECTOR_INIT_CAPACITY);
  v->size = 0;
  v->cap = VECTOR_INIT_CAPACITY;
  v->free = f;
  v->cmp = c;
  return v;
}

void vector_destroy(vector_t *v) {
  if (!v) return;
  if (v->free)
    while (v->size--)
      v->free(v->data[v->size]);
  free(v);
}

/*******************************************************/
free_func_t vector_set_free(vector_t *v, free_func_t f) {
  free_func_t old = v->free;
  v->free = f;
  return old;
}

cmp_func_t vector_set_cmp(vector_t *v, cmp_func_t c) {
  cmp_func_t old = v->cmp;
  v->cmp = c;
  return old;
}

/*******************************************************/
size_t vector_size(const vector_t *v) {
  return v ? v->size : 0;
}

size_t vector_capacity(const vector_t *v) {
  return v ? v->cap : 0;
}

void ** vector_raw(vector_t *v) {
  return v ? v->data : NULL;
}

/*******************************************************/
vector_t * vector_resize(vector_t *v, const size_t size) {
  if (!v) return NULL;
  if (size < v->size) {
    if (v->free) {
      for (size_t i = size; i < v->size; ++i)
        v->free(v->data[i]);
    }
    memset(v->data + size, 0, sizeof(void*) * (v->size - size));
  }
  if (size > v->cap ||
      (v->cap > VECTOR_INIT_CAPACITY && size < v->cap / 2)) {
    size_t cap = size + (size >> 3) + (size < 9 ? 3 : 6);
    v = (vector_t*)xrealloc(v, sizeof(vector_t) + sizeof(void*) * cap);
    if (!v) return NULL;
    if (cap > v->cap)
      memset(v->data + v->cap, 0, sizeof(void*) * (cap - v->cap));
    v->cap = cap;
  }
  v->size = size;
  return v;
}

/*******************************************************/
void * vector_get(const vector_t *v, ssize_t idx) {
  if (!v) return NULL;
  if (idx < 0) idx += v->size;
  if (idx < 0 || (size_t)idx >= v->size) return NULL;
  return v->data[idx];
}

void vector_set(vector_t *v, ssize_t idx, void *data) {
  if (!v) return;
  if (idx < 0) idx += v->size;
  if (idx < 0 || (size_t)idx >= v->size) return;
  if (v->free) v->free(v->data[idx]);
  v->data[idx] = data;
}

/*******************************************************/
vector_t * vector_append(vector_t *v, void *data) {
  return vector_insert(v, v->size, data);
}

vector_t * vector_insert(vector_t *v, ssize_t idx, void *data) {
  if (!v) return NULL;
  if (idx < 0) idx += v->size;
  if (idx < 0 || (size_t)idx > v->size) return NULL;
  v = vector_resize(v, v->size + 1);
  if (!v) return NULL;
  if ((size_t)idx < v->size - 1)
    memmove(v->data + idx + 1, v->data + idx,
            sizeof(void*) * (v->size - 1 - idx));
  v->data[idx] = data;
  return v;
}

vector_t * vector_remove(vector_t *v, ssize_t idx) {
  if (!v) return NULL;
  if (idx < 0) idx += v->size;
  if (idx < 0 || (size_t)idx >= v->size) return NULL;
  if (v->free) v->free(v->data[idx]);
  if ((size_t)idx < v->size - 1)
    memmove(v->data + idx, v->data + idx + 1,
            sizeof(void*) * (v->size - 1 - idx));
  v->data[v->size-1] = NULL;
  return vector_resize(v, v->size - 1);
}

/*******************************************************/
ssize_t vector_find(const vector_t *v, const void *data) {
  if (!v) return -1;
  if (!v->cmp) {
    fprintf(stderr, "vector find error: no comparison function\n");
    return -1;
  }
  for (size_t i = 0; i < v->size; ++i)
    if (v->cmp(v->data[i], data) == 0)
      return i;
  return -1;
}

void vector_foreach(const vector_t *v, void (*f)(void*)) {
  if (!v || !f)
    return;
  for (size_t i = 0; i < v->size; ++i)
    f(v->data[i]);
}

/* vim: set sw=2 sts=2 : */
