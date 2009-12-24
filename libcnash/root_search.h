#ifndef _ROOT_SEARCH_H_
#define _ROOT_SEARCH_H_

#include <cnash/cnash.h>
#include <cnash/function.h>

extern int root_search_verbose;

int root_secant(function_t *f, interval_t *i, stop_cond_t *s, double *r);
int root_newton(function_t *f, double x0, stop_cond_t *s, double *r);
int root_bisection(function_t *f, interval_t *i, stop_cond_t *s, double *r);
int root_regulafalsi(function_t *f, interval_t *i, stop_cond_t *s, double *r);

#endif /* _ROOT_SEARCH_H_ */

/* vim: set sw=2 sts=2 : */
