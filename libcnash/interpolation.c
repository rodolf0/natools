#include "cnash.h"
#include "interpolation.h"
#include "combinatorics.h"

/* Newton's interpolation polynomial:
 *
 * x0  y0 = f[x0]
 *
 *          y1 - y0
 * x1  y1  --------- = f[x0,x1]
 *          x1 - x0
 *          y2 - y1               f[x0,x2] - f[x0,x1]
 * x2  y2  --------- = f[x0,x2]  --------------------- = f[x0,x1,x2]
 *          x2 - x1                    x2  - x0
 *
 *         n                   i-1
 * N(x) = SUM f[x0, ..., xi] * MUL (x - xj) = f[x0] + f[x0,x1] * (x - x0) ...
 *        i=0                  j=0
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
  /* x[i].x1 have now f[x0, ..., xi] */
  return 0;
}

/* central nth finite difference is prefered because lower error
 *
 * d^nf    nth_central_diff f(x)
 * ---- = ----------------------- + O(h^2)
 * dx^n           h^n
 *
 * */
int finite_difference(function_t *f, int n, double x0, double h, double *r) {
  int i, sign;
  double fx_i;
  /* T = 1:   forward difference
   * T = 0.5: central difference
   * T = 0:   backward difference */
  double T = 0.5;

  for (i = 0, sign = 1, *r = 0.0; i <= n; i++) {
    if (evaluate_function(f, x0 + (T*(double)n - i) * h, &fx_i)) return 1;
    *r += sign * k_combinations(n, i) * fx_i;
    sign *= -1;
  }
  return 0;
}


/*         n        n    x - xj
 * L(x) = SUM yi * MUL  ---------  j!=i
 *        i=0      j=0   xi - xj
 * */
int interpolate_lagrange(vector2_t *x, int n, double x0, double *r) {
  double p;
  int i, j;

  for (i = 0, *r = 0.0; i < n; i++) {
    for (j = 0, p = 1.0; j < n; j++)
      if (j != i)
        p *= (x0 - x[j].x0) / (x[i].x0 - x[j].x0);
    *r += x[i].x1 * p;
  }
  return 0;
}

/* when (x0, ..., xn) -> (z, .., z) N(x) -> T(x)
 * (http://en.wikipedia.org/wiki/Umbral_calculus)
 *
 *        inf  D(f,i)(a)
 * f(x) = SUM ----------- * (x - a)^i
 *        i=0      i!
 *
 * D(f,i) is the ith derivate of f
 * */
function_t *aproximate_taylor();


/* vim: set sw=2 sts=2 : */
