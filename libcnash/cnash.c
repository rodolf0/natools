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

/* vim: set sw=2 sts=2 : */
