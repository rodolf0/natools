#include <stdlib.h>

#include "cnash.h"

interval_t *interval_create(double x0, double x1) {
  interval_t *i = (interval_t*)malloc(sizeof(interval_t));
  i->x0 = x0;
  i->x1 = x1;
  return i;
}

void interval_destroy(interval_t *i) {
  if (i)
    free(i);
}

void interval_swap(interval_t *i) {
  double m = i->x0;
  i->x0 = i->x1;
  i->x1 = m;
}

stop_cond_t *stop_cond_create(double e, int max_i) {
  stop_cond_t *s = (stop_cond_t*)malloc(sizeof(stop_cond_t));
  s->epsilon = e;
  s->max_iterations = max_i;
  return s;
}

void stop_cond_destroy(stop_cond_t *s) {
  if (s)
    free(s);
}

double *vector_at(vector_t *v, size_t n) {
  return v->a + n;
}

matrix_t *matrix_create(size_t r, size_t c) {
  matrix_t *m = (matrix_t*)malloc(sizeof(matrix_t));
  m->a = (double*)malloc(sizeof(double) * r * c);
  m->r = r;
  m->c = c;
  return m;
}

void matrix_destroy(matrix_t *m) {
  if (m) {
    if (m->a)
      free(m->a);
    free(m);
  }
}

double *matrix_at(matrix_t *m, size_t r, size_t c) {
  return m->a + r * m->c + c;
}

/* vim: set sw=2 sts=2 : */
