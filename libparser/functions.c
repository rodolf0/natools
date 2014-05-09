/* use GNU_SOURCE for long double constants */
#ifndef __USE_GNU
#define __USE_GNU
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <math.h>
#include <stdlib.h>

#include "parser-priv.h"

static inline long double pop_arg(list_t *args) {
  long double *a = (long double*)list_pop(args);
  long double r = *a;
  free(a);
  return r;
}

long double _max(list_t *args, size_t n) {
  long double tmp, max = pop_arg(args);
  while (--n)
    if ((tmp = pop_arg(args)) > max)
      max = tmp;
  return max;
}

long double _min(list_t *args, size_t n) {
  long double tmp, min = pop_arg(args);
  while (--n)
    if ((tmp = pop_arg(args)) < min)
      min = tmp;
  return min;
}

long double _sum(list_t *args, size_t n) {
  long double sum = 0.0;
  while (n--)
    sum += pop_arg(args);
  return sum;
}

long double _avg(list_t *args, size_t n) {
  return _sum(args, n) / (long double)n;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
long double _gamma(list_t *args, size_t n) {
  return tgammal(pop_arg(args));
}

long double _random(list_t *args, size_t n) {
  return (long double)random();
}

long double _abs(list_t *args, size_t n) {
  return fabsl(pop_arg(args));
}

long double _sin(list_t *args, size_t n) {
  return sinl(pop_arg(args));
}

long double _cos(list_t *args, size_t n) {
  return cosl(pop_arg(args));
}

long double _tan(list_t *args, size_t n) {
  return tanl(pop_arg(args));
}

long double _asin(list_t *args, size_t n) {
  return asinl(pop_arg(args));
}

long double _acos(list_t *args, size_t n) {
  return acos(pop_arg(args));
}

long double _atan(list_t *args, size_t n) {
  return atanl(pop_arg(args));
}

long double _atan2(list_t *args, size_t n) {
  long double x, y;
  y = pop_arg(args);
  x = pop_arg(args);
  return atan2l(x, y);
}

long double _log(list_t *args, size_t n) {
  return logl(pop_arg(args));
}

long double _exp(list_t *args, size_t n) {
  return expl(pop_arg(args));
}

long double _round(list_t *args, size_t n) {
  return roundl(pop_arg(args));
}
#pragma GCC diagnostic pop

void register_functions(hashtbl_t *h) {
  hashtbl_insert(h, "max(", (void*)_max);
  hashtbl_insert(h, "min(", (void*)_min);
  hashtbl_insert(h, "sum(", (void*)_sum);
  hashtbl_insert(h, "avg(", (void*)_avg);
  hashtbl_insert(h, "random(", (void*)_random);
  hashtbl_insert(h, "abs(", (void*)_abs);

  hashtbl_insert(h, "sin(", (void*)_sin);
  hashtbl_insert(h, "cos(", (void*)_cos);
  hashtbl_insert(h, "tan(", (void*)_tan);
  hashtbl_insert(h, "asin(", (void*)_asin);
  hashtbl_insert(h, "acos(", (void*)_acos);
  hashtbl_insert(h, "atan(", (void*)_atan);
  hashtbl_insert(h, "atan2(", (void*)_atan2);
  hashtbl_insert(h, "log(", (void*)_log);
  hashtbl_insert(h, "exp(", (void*)_exp);

  hashtbl_insert(h, "gamma(", (void*)_gamma);
  hashtbl_insert(h, "round(", (void*)_round);
}

void register_constants(hashtbl_t *h) {
  long double *x;
  x = (long double*)zmalloc(sizeof(long double)); *x = M_PI;
  hashtbl_insert(h, "pi", x);

  x = (long double*)zmalloc(sizeof(long double)); *x = M_E;
  hashtbl_insert(h, "e", x);

  x = (long double*)zmalloc(sizeof(long double)); *x = (1.0 + sqrtl(5)) / 2.0;
  hashtbl_insert(h, "phi", x);

  x = (long double*)zmalloc(sizeof(long double)); *x = 27021984;
  hashtbl_insert(h, "_stashed", x);
}

/* vim: set sw=2 sts=2 : */
