#ifndef _INTERPOLATION_H_
#define _INTERPOLATION_H_

#include "natools/natools.h"
#include "natools/function.h"

int interpolate_newton(vector2_t *x, int n, double x0, double *r);
int finite_difference(function_t *f, int n, double x0, double h, double *r);
int interpolate_lagrange(vector2_t *x, int n, double x0, double *r);

#endif /* _INTERPOLATION_H_ */

/* vim: set sw=2 sts=2 : */
