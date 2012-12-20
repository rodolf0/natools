#ifndef _ROOT_SEARCH_H_
#define _ROOT_SEARCH_H_

#include "natools/natools.h"
#include "natools/function.h"

extern int root_search_verbose;

int root_secant(function_t *f, interval_t *i, stop_cond_t *s, long double *r);
int root_newton(function_t *f, long double x0, stop_cond_t *s, long double *r);
int root_bisection(function_t *f, interval_t *i, stop_cond_t *s, long double *r);
int root_regulafalsi(function_t *f, interval_t *i, stop_cond_t *s, long double *r);

#endif /* _ROOT_SEARCH_H_ */

/* vim: set sw=2 sts=2 : */
