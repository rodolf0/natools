#include <stdlib.h>
#include <string.h>
#include "baas/vector.h"

#define VECTOR_INIT_CAPACITY 32


vector_t * vector_init(free_func_t f, cmp_func_t c) {
  vector_t *v = (vector_t*)malloc(sizeof(vector_t));
  v->cap = VECTOR_INIT_CAPACITY;
  v->size = 0;
  v->data = malloc(v->cap * sizeof(void*));
  v->free = f;
  v->cmp = c;
  return v;
}


vector_t * vector_copy(vector_t *c) {
  if (!c)
    return NULL;
  vector_t *v = (vector_t*)malloc(sizeof(vector_t));
  v->cap = c->cap;
  v->size = c->size;
  v->data = malloc(v->cap * sizeof(void*));
  v->free = c->free;
  v->cmp = c->cmp;
  memcpy(v->data, c->data, sizeof(void*) * v->size);
  return v;
}


void vector_destroy(vector_t *v) {
  if (!v)
    return;
  if (v->free)
    while (v->size--)
      v->free(v->data[v->size]);
  free(v->data);
  free(v);
}


/* automatic resize */
static inline void vector_recapcitate(vector_t *v) {
  if (!v)
    return;
  /* TODO: check realloc return value (may overwrite our cur value if fail) */
  if (v->size == v->cap) {
    v->cap *= 2;
    v->data = realloc(v->data, sizeof(void*) * v->cap);
  } else if (v->cap > VECTOR_INIT_CAPACITY && v->size < v->cap/3) {
    v->cap = (v->cap/2 < VECTOR_INIT_CAPACITY ? VECTOR_INIT_CAPACITY:v->cap/2);
    v->data = realloc(v->data, sizeof(void*) * v->cap);
  }
}


ssize_t vector_insert(vector_t *v, ssize_t idx, void *data) {
  if (!v)
    return -1;
  /* check [-size <= idx <= size] */
  if (-idx > (ssize_t)v->size || idx > (ssize_t)v->size)
    return -2;
  if (idx < 0)
    idx += v->size;
  vector_recapcitate(v);
  /* make room for the new element */
  memmove(v->data + idx + 1, v->data + idx, sizeof(void*) * (v->size - idx));
  v->data[idx] = data;
  v->size++;
  return idx;
}


void vector_remove(vector_t *v, ssize_t idx) {
  if (!v)
    return;
  /* check [-size <= idx < size] */
  if (-idx > (ssize_t)v->size || idx >= (ssize_t)v->size)
    return;
  if (idx < 0)
    idx += v->size;
  vector_recapcitate(v);
  /* overwrite the old element */
  memmove(v->data + idx, v->data + idx + 1, sizeof(void*) * (v->size-idx-1));
  v->size--;
}


void * vector_get(vector_t *v, ssize_t idx) {
  if (!v)
    return NULL;
  /* check [-size <= idx < size] */
  if (-idx > (ssize_t)v->size || idx >= (ssize_t)v->size)
    return NULL;
  if (idx < 0)
    idx += v->size;
  return v->data[idx];
}


void vector_set(vector_t *v, ssize_t idx, void *data) {
  if (!v)
    return;
  /* check [-size <= idx < size] */
  if (-idx > (ssize_t)v->size || idx >= (ssize_t)v->size)
    return;
  if (idx < 0)
    idx += v->size;
  if (v->free)
    v->free(v->data[idx]);
  v->data[idx] = data;
}


void vector_resize(vector_t *v, const size_t len) {
  if (!v)
    return;
  while (v->size > len) {
    if (v->free)
      v->free(v->data[--v->size]);
  }
  if (len > v->cap) {
    while (len > v->cap)
      v->cap *= 2;
    v->data = realloc(v->data, sizeof(void*) * v->cap);
  }
  if (len > v->size) {
    memset(v->data + v->size, 0, sizeof(void*) * (len - v->size));
    v->size = len;
  }
}


ssize_t vector_find(vector_t *v, void *data) {
  if (!v || !v->cmp)
    return -2;
  size_t i;
  for (i = 0; i < v->size; i++)
    if (v->cmp(v->data[i], data) == 0)
      return i;
  return -1;
}


void vector_foreach(vector_t *v, void (*f)(void*)) {
  if (!v || !f)
    return;
  size_t i;
  for (i = 0; i < v->size; i++)
    (*f)(v->data[i]);
}

/* vim: set sw=2 sts=2 : */
