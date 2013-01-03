#include <math.h>

#include "na/calculus.h"
#include "na/interpolation.h"


const long double h = 1.0e-5;

/* calculates de derivate of f at f(x0)
 *             _______
 * |         _/
 * |       _/
 * |      /
 * |     /
 * |______________________
 *    x0-h   x0    x0+h
 *
 * a good h is h = sqrt(epsilon) * x0
 * */

/* five point stencil */
long double derivate_1(function_t *f, long double x0) {
  long double p0, p1, p2, p3;
  p0 = function_eval(f, x0 - 2.0 * h);
  p1 = function_eval(f, x0 - 1.0 * h);
  p2 = function_eval(f, x0 + 1.0 * h);
  p3 = function_eval(f, x0 + 2.0 * h);
  return (p0 + 8.0 * (p2 - p1) - p3) / (12.0 * h);
}

long double derivate_2(function_t *f, long double x0) {
  long double p0, p1, p2, p3, p4;
  p0 = function_eval(f, x0 - 2.0 * h);
  p1 = function_eval(f, x0 - 1.0 * h);
  p2 = function_eval(f, x0);
  p3 = function_eval(f, x0 + 1.0 * h);
  p4 = function_eval(f, x0 + 2.0 * h);
  return (-p4 + 16.0 * (p3 + p1) - 30.0 * p2 - p0) / (12.0 * h * h);
}

long double derivate(function_t *f, int n, long double x0) {
  const long double _h = 1.5e-2;
  return finite_difference(f, n, x0, _h) / powl(_h, n);
}


/* composite simpson's rule:
 * n: the resolution points used to calculate (actually half of the points) */
long double integrate_simpson(function_t *f, long double x0, long double x1) {
  int j, n = (int)((x1 - x0) / 1.0e-2);
  long double _h = (x1 - x0) / (2.0 * n);
  long double p0, pm, pi = 0.0, pj = 0.0;

  p0 = function_eval(f, x0);
  pm = function_eval(f, x1);

  for (j = 1; j < n; j++)
    pj += function_eval(f, x0 + _h * (2*j));
  for (j = 1; j <= n; j++)
    pi += function_eval(f, x0 + _h * (2*j-1));

  return (p0 + 2.0 * pj + 4.0 * pi + pm) * _h / 3.0;
}


/*
 *     /x1
 * l = | sqrt(1 + f'(x)^2)dx
 *     /x0
 * Compute the arc-length using composite simpson's rule
 */
long double arc_length(function_t *f, long double x0, long double x1) {
  int j, n = (int)((x1 - x0) / 0.3e-2);
  long double _h = (x1 - x0) / (2.0 * n);
  long double p0, pm, pi = 0.0, pj = 0.0;

  p0 = derivate_1(f, x0);
  p0 = sqrtl(1.0 + p0 * p0);
  pm = derivate_1(f, x1);
  pm = sqrtl(1.0 + pm * pm);

  for (j = 1; j < n; j++) {
    long double d = derivate_1(f, x0 + _h * (2*j));
    pj += sqrtl(1.0 + d * d);
  }
  for (j = 1; j <= n; j++) {
    long double d = derivate_1(f, x0 + _h * (2*j-1));
    pi += sqrtl(1.0 + d * d);
  }

  return (p0 + 2.0 * pj + 4.0 * pi + pm) * _h / 3.0;
}

/* vim: set sw=2 sts=2 : */
