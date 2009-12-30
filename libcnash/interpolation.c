#include "cnash.h"

/* Newton's interpolation polynomial:
 * x0  y0
 *
 *          y1 - y0
 * x1  y1  --------- = y1'
 *          x1 - x0
 *          y2 - y1          y2' - y1'
 * x2  y2  --------- = y2'  ----------- = y1"
 *          x2 - x1          x2  - x0
 *
 * N(x) = y0 + y1' * (x - x0) + y1" * (x - x0) * (x - x1)
 * */

int interpolate_newton(vector2_t *x, int n, double x0, double *r) {
  double a;
  int i, j;

  for (i = 0, *r = 0.0, a = 1.0; i < n; i++) {

    /* x[i].x1 = f[x0, ..., xi]: devided difference */
    for (j = i; j < n && j > 0; j++)
      x[j].x1 = (x[j].x1 - x[j-1].x1) / (x[j].x0 - x[j-i].x0);

    *r += x[i].x1 * a;
    a *= x0 - x[i].x0; /* (x - x0) * ... * (x - xi) */
  }

  return 0;
}

/* when (x0, ..., xn) -> (z, .., z) N(x) -> T(x)
 * Taylor Series:
 *
 *        inf  D(f,n)(a)
 * f(x) = SUM ----------- * (x - a)^n
 *        n=0      n!
 *
 * D(f,n) is the nth derivate of f
 * */

/* vim: set sw=2 sts=2 : */
