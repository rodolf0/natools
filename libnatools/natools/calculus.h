#ifndef _CALCULUS_H_
#define _CALCULUS_H_

#include "natools/natools.h"
#include "natools/function.h"

int derivate_3p(function_t *f, long double x0, long double h, long double *r);
int derivate_5p(function_t *f, long double x0, long double h, long double *r);

/* composite simpson's rule:
 * n: the resolution points used to calculate
 * actually half of the points) */
int integrate_simpson(function_t *f, interval_t *i, int n, long double *r);

#endif /* _CALCULUS_H_  */

/* vim: set sw=2 sts=2 : */
