#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "na/root_finding.h"
#include "na/calculus.h"

int root_search_verbose = 0;

/*                 f(X_n)
 * X_n+1 = X_n - --------- (Newton's method)
 *                f'(X_n)
 *
 *            f(X_n) - f(X_n-1)
 * f'(X_n) ~ -------------------
 *               X_n - X_n-1
 *
 *                   X_n - X_n-1
 * X_n+1 = X_n - ------------------- * f(X_n)  (Secant method)
 *                f(X_n) - f(X_n-1)
 * */
int root_secant(function_t *f, interval_t *i, stop_cond_t *s, long double *r) {
  long double d, f1, f0;
  size_t j;

  f0 = function_eval(f, i->x0);
  f1 = function_eval(f, i->x1);

  for (j = 0; j < s->max_iterations; j++) {
    /* calculate next aproximation */
    d = f1 * (i->x1 - i->x0) / (f1 - f0);
    /* check stop condition */
    if (fabsl(d) < s->epsilon) break;

    /* update secant points */
    i->x0 = i->x1; f0 = f1;
    i->x1 = i->x1 - d;
    f1 = function_eval(f, i->x1);

    if (root_search_verbose)
      fprintf(stderr, "i=%zu: x=%.15Lg f(x)=%.15Lg d=%.15Lg\n", j, i->x1, f1, d);
  }

  /* aproximate root */
  *r = i->x1;
  return 0;
}

int root_newton(function_t *f, long double x0, stop_cond_t *s, long double *r) {
  long double f0, df0_dx, d;
  size_t j;

  for (j = 0; j < s->max_iterations; j++) {
    /* evaluate function at x0 */
    f0 = function_eval(f, x0);
    /* evaluate f'(x) at x0 */
    df0_dx = derivate_1(f, x0);
    /* check stop condition */
    if (fabsl(d = f0 / df0_dx) < s->epsilon) break;

    x0 -= d;

    if (root_search_verbose)
      fprintf(stderr, "i=%zu: x=%.15Lg f(x)=%.15Lg d=%.15Lg\n", j, x0, f0, d);
  }

  *r = x0;
  return 0;
}

static void pair_swap(long double *x0, long double *x1) {
  long double m = *x0;
  *x0 = *x1; *x1 = m;
}

int root_bisection(function_t *f, interval_t *i, stop_cond_t *s, long double *r) {
  long double m, fm, f1, f0;
  size_t j;

  f0 = function_eval(f, i->x0);
  f1 = function_eval(f, i->x1);

  /* f(x0) and f(x1) have opposite signs => contain a root */
  if (f0 > 0.0 && f1 < 0.0) {
    interval_swap(i);
    pair_swap(&f0, &f1);
  } else if (!(f0 < 0.0 && f1 > 0.0)) {
    fprintf(stderr, "f(x0) * f(x1) !< 0\n");
    return 3;
  }

  for (j = 0; j < s->max_iterations; j++) {
    /* evaluate function at midpoint */
    m = (i->x0 + i->x1) / 2.0;
    fm = function_eval(f, m);
    /* check stop conditions */
    if (i->x0 == m || i->x1 == m ||
        fabsl(i->x0 - i->x1) < s->epsilon) break;

    /* update endpoints */
    if (fm < 0.0) {
      i->x0 = m; f0 = fm;
    } else {
      i->x1 = m; f1 = fm;
    }

    if (root_search_verbose)
      fprintf(stderr, "i=%zu: x=%.15Lg f(x0)=%.15Lg f(x1)=%.15Lg\n",
              j, (i->x1 + i->x0) / 2.0, f0, f1);
  }

  *r = (i->x0 + i->x1) / 2.0;
  return 0;
}

/* x0 and x1 are the endpoints for the search interval
 * Aproximate the the root of the function by finding
 * the root of the secant.
 *
 * down-weighting is applied to improve method and
 * make it suprlinear convergent.
 * */
int root_regulafalsi(function_t *f, interval_t *i, stop_cond_t *s, long double *r) {
  long double m = 0.0, fm, f1, f0;
  size_t j;
  ssize_t side = 0;

  f0 = function_eval(f, i->x0);
  f1 = function_eval(f, i->x1);

  /* f(x0) and f(x1) have opposite signs => contain a root */
  if (f0 > 0.0 && f1 < 0.0) {
    interval_swap(i);
    pair_swap(&f0, &f1);
  } else if (!(f0 < 0.0 && f1 > 0.0)) {
    fprintf(stderr, "f(x0) * f(x1) !< 0\n");
    return 3;
  }

  for (j = 0; j < s->max_iterations; j++) {
    /* get the root of the secant */
    m = (f1 * i->x0 - f0 * i->x1) / (f1 - f0);
    /* check stop conditions */
    if (fabsl(i->x0 - i->x1) < s->epsilon * fabsl(i->x0 + i->x1))
      break;

    fm = function_eval(f, m);

    /* update endpoints */
    if (fm < 0.0) {
      if (side == -1) f1 /= 2.0;
      i->x0 = m; f0 = fm;
      side = -1;
    } else if (fm > 0.0) {
      /* down-weight x0 side to avoid
       * linear convergence because of choosing
       * always the same side */
      if (side == 1) f0 /= 2.0;
      i->x1 = m; f1 = fm;
      side = 1;
    } else
      break;

    if (root_search_verbose)
      fprintf(stderr, "i=%zu: x=%.15Lg f(x)=%.15Lg\n", j, m, fm);
  }

  *r = m;
  return 0;
}

/* System of linear equations */

/* Solve a system of linear equations using Gauss-Seidel method
 * A: the coefficients (A[i,i] shouldn't be 0)
 * x: sussesive aproximations
 */
int roots_gauss_seidel(matrix_t *A, vector_t *x, vector_t *b, stop_cond_t *s) {
  size_t i, j, k;
  long double *xi, *xj, *mij, *bi, *mii, d;

  for (k = 0; k < s->max_iterations; k++) {
    for (i = 0, d = 0.0; i < A->r; i++) {

      for (j = 0; j < A->r; j++) {
        if (j == i) continue; /*  j != i */
        mij = matrix_at(A, i, j);
        xj = vector_at(x, j);
        d += *mij * *xj;
      }
      xi = vector_at(x, i);
      bi = vector_at(b, i);
      mii = matrix_at(A, i, i);

      *xi = (*bi - d) / *mii;
    }

    /* TODO: check stop condition */
    /* || b - A * x || / || b || < e */
  }
  return 0;
}

/* vim: set sw=2 sts=2 : */
