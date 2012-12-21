/* use GNU_SOURCE for long double */
#define __USE_GNU
#define _GNU_SOURCE
#include <math.h>
#include <stdlib.h>

#include "parser-priv.h"

int _max(parser_t *p, size_t nargs, long double *r) {
  long double max; 
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  while (--nargs) {
    if ((e = pop_operand(p, &max)))
      return e;
    if (*r < max)
      *r = max;
  }
  return 0;
}

int _min(parser_t *p, size_t nargs, long double *r) {
  long double min; 
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  while (--nargs) {
    if ((e = pop_operand(p, &min)))
      return e;
    if (*r > min)
      *r = min;
  }
  return 0;
}

int _sum(parser_t *p, size_t nargs, long double *r) {
  long double d;
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  while (--nargs) {
    if ((e = pop_operand(p, &d)))
      return e;
    *r += d;
  }
  return 0;
}

int _avg(parser_t *p, size_t nargs, long double *r) {
  int e = _sum(p, nargs, r);
  *r /= (long double)nargs;
  return e;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int _gamma(parser_t *p, size_t nargs, long double *r) {
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  *r = tgammal(*r);
  return 0;
}

int _random(parser_t *p, size_t nargs, long double *r) {
  *r = (long double)random();
  return 0;
}

int _abs(parser_t *p, size_t nargs, long double *r) {
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  *r = fabsl(*r);
  return 0;
}

int _sin(parser_t *p, size_t nargs, long double *r) {
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  *r = sinl(*r);
  return 0;
}

int _cos(parser_t *p, size_t nargs, long double *r) {
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  *r = cosl(*r);
  return 0;
}

int _tan(parser_t *p, size_t nargs, long double *r) {
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  *r = tanl(*r);
  return 0;
}

int _asin(parser_t *p, size_t nargs, long double *r) {
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  *r = asinl(*r);
  return 0;
}

int _acos(parser_t *p, size_t nargs, long double *r) {
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  *r = acosl(*r);
  return 0;
}

int _atan(parser_t *p, size_t nargs, long double *r) {
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  *r = atanl(*r);
  return 0;
}

int _atan2(parser_t *p, size_t nargs, long double *r) {
  int e;
  long double x, y;
  if ((e = pop_operand(p, &y)))
    return e;
  if ((e = pop_operand(p, &x)))
    return e;
  *r = atan2l(x, y);
  return 0;
}

int _log(parser_t *p, size_t nargs, long double *r) {
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  *r = logl(*r);
  return 0;
}

int _exp(parser_t *p, size_t nargs, long double *r) {
  int e;
  if ((e = pop_operand(p, r)))
    return e;
  *r = expl(*r);
  return 0;
}
#pragma GCC diagnostic pop

int register_functions(parser_t *p) {
  if (!p)
    return 1;

  hashtbl_insert(p->function_table, "max(", _max);
  hashtbl_insert(p->function_table, "min(", _min);
  hashtbl_insert(p->function_table, "sum(", _sum);
  hashtbl_insert(p->function_table, "avg(", _avg);
  hashtbl_insert(p->function_table, "random(", _random);
  hashtbl_insert(p->function_table, "abs(", _abs);

  hashtbl_insert(p->function_table, "sin(", _sin);
  hashtbl_insert(p->function_table, "cos(", _cos);
  hashtbl_insert(p->function_table, "tan(", _tan);
  hashtbl_insert(p->function_table, "asin(", _asin);
  hashtbl_insert(p->function_table, "acos(", _acos);
  hashtbl_insert(p->function_table, "atan(", _atan);
  hashtbl_insert(p->function_table, "atan2(", _atan2);
  hashtbl_insert(p->function_table, "log(", _log);
  hashtbl_insert(p->function_table, "exp(", _exp);

  hashtbl_insert(p->function_table, "gamma(", _gamma);

  return 0;
}

/*int register_constants(parser_t *p) {*/
  /*long double *x = malloc(sizeof(long double));*/
  /*srandom((unsigned int)*x); [> use heap garbage as random seed <]*/
  /*parser_setvar(p, "ans", *x);*/
  /*parser_setvar(p, "pi", M_PIl);*/
  /*parser_setvar(p, "e", M_El);*/
  /*parser_setvar(p, "phi", (1.0 + sqrtl(5)) / 2.0);*/
  /*return 0;*/
/*}*/

/* vim: set sw=2 sts=2 : */
