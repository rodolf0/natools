#include <math.h>
#include "natools/natools.h"
#include "natools/calculus.h"
#include "natools/interpolation.h"

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
int derivate_3p(function_t *f, long double x0, long double h, long double *r) {
  long double p0, p1;

  p0 = function_eval(f, x0 - h);
  p1 = function_eval(f, x0 + h);

  *r = (p1 - p0) / (2.0 * h);
  return 0;
}

int derivate_5p(function_t *f, long double x0, long double h, long double *r) {
  long double p0, p1, p2, p3;

  p0 = function_eval(f, x0 - 2.0 * h);
  p1 = function_eval(f, x0 - 1.0 * h);
  p2 = function_eval(f, x0 + 1.0 * h);
  p3 = function_eval(f, x0 + 2.0 * h);

  *r = (p0 + 8.0 * (p2 - p1) - p3) / (12.0 * h);
  return 0;
}

/* TODO: extend this to a 5 point aproach ? */
int derivate(function_t *f, int n, long double x0, long double h, long double *r) {
  if (finite_difference(f, n, x0, h, r)) return 1;
  *r = *r / pow(h, n);
  return 0;
}

/* composite simpson's rule:
 * n: the resolution points used to calculate
 * actually half of the points) */
int integrate_simpson(function_t *f, interval_t *i, int n, long double *r) {
  long double h = (i->x1 - i->x0) / (2.0 * n);
  long double p0, pm, pi, pj, fj;
  int j;

  p0 = function_eval(f, i->x0);
  pm = function_eval(f, i->x1);

  for (j = 1, pj = 0.0; j < n; j++) {
    fj = function_eval(f, i->x0 + h * (2*j));
    pj += fj;
  }
  for (j = 1, pi = 0.0; j <= n; j++) {
    fj = function_eval(f, i->x0 + h * (2*j-1));
    pi += fj;
  }

  *r = (p0 + 2.0 * pj + 4.0 * pi + pm) * h / 3.0;
  return 0;
}

/* vim: set sw=2 sts=2 : */
