#ifndef _CALCULUS_H_
#define _CALCULUS_H_

#include <cnash/cnash.h>
#include <cnash/function.h>

int derivate_3p(function_t *f, double x0, double h, double *r);
int derivate_5p(function_t *f, double x0, double h, double *r);

/* composite simpson's rule:
 * n: the resolution points used to calculate
 * actually half of the points) */
int integrate_simpson(function_t *f, interval_t *i, int n, double *r);

#endif /* _CALCULUS_H_  */

/* vim: set sw=2 sts=2 : */
