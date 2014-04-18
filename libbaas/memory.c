#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void * xmalloc(size_t size) {
  void *r = malloc(size);
  if (r == NULL) {
    fprintf(stderr, "xmalloc: failed\n");
  }
  return r;
}

void * zmalloc(size_t size) {
  void *r = xmalloc(size);
  if (r != NULL) {
    memset(r, 0, size);
  }
  return r;
}

void * xrealloc(void *m, size_t size) {
  void *r = realloc(m, size);
  if (r == NULL) {
    fprintf(stderr, "xrealloc: failed, mem freed\n");
    free(m);
  }
  return r;
}

void xfree(void *m) {
  free(m);
}

/* vim: set sw=2 sts=2 : */
