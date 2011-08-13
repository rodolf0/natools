#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "natools/calculus.h"
#include "natools/equation_solver.h"

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
int root_secant(function_t *f, interval_t *i, stop_cond_t *s, double *r) {
  double d, f1, f0;
  int j;

  if (evaluate_function(f, i->x0, &f0)) return 1;
  if (evaluate_function(f, i->x1, &f1)) return 1;

  for (j = 0; j < s->max_iterations; j++) {
    /* calculate next aproximation */
    d = f1 * (i->x1 - i->x0) / (f1 - f0);
    /* check stop condition */
    if (fabs(d) < s->epsilon) break;

    /* update secant points */
    i->x0 = i->x1; f0 = f1;
    i->x1 = i->x1 - d;
    if (evaluate_function(f, i->x1, &f1)) return 1;

    if (root_search_verbose)
      fprintf(stderr, "i=%d: x=%.15f f(x)=%.15f d=%.15f\n", j, i->x1, f1, d);
  }

  /* aproximate root */
  *r = i->x1;
  return 0;
}

int root_newton(function_t *f, double x0, stop_cond_t *s, double *r) {
  double f0, df0_dx, d;
  int j;

  for (j = 0; j < s->max_iterations; j++) {
    /* evaluate function at x0 */
    if (evaluate_function(f, x0, &f0)) return 1;
    /* evaluate f'(x) at x0 */
    if (derivate_5p(f, x0, 0.05, &df0_dx)) return 2;
    /* check stop condition */
    if (fabs(d = f0 / df0_dx) < s->epsilon) break;

    x0 -= d;

    if (root_search_verbose)
      fprintf(stderr, "i=%d: x=%.15f f(x)=%.15f d=%.15f\n", j, x0, f0, d);
  }

  *r = x0;
  return 0;
}

static void pair_swap(double *x0, double *x1) {
  double m = *x0;
  *x0 = *x1; *x1 = m;
}

int root_bisection(function_t *f, interval_t *i, stop_cond_t *s, double *r) {
  double m, fm, f1, f0;
  int j;

  if (evaluate_function(f, i->x0, &f0)) return 1;
  if (evaluate_function(f, i->x1, &f1)) return 1;

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
    if (evaluate_function(f, m, &fm)) return 1;
    /* check stop conditions */
    if (i->x0 == m || i->x1 == m ||
        fabs(i->x0 - i->x1) < s->epsilon) break;

    /* update endpoints */
    if (fm < 0.0) {
      i->x0 = m; f0 = fm;
    } else {
      i->x1 = m; f1 = fm;
    }

    if (root_search_verbose)
      fprintf(stderr, "i=%d: x=%.15f f(x0)=%.15f f(x1)=%.15f\n",
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
int root_regulafalsi(function_t *f, interval_t *i, stop_cond_t *s, double *r) {
  double m = 0.0, fm, f1, f0;
  int j, side = 0;

  if (evaluate_function(f, i->x0, &f0)) return 1;
  if (evaluate_function(f, i->x1, &f1)) return 1;

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
    if (fabs(i->x0 - i->x1) < s->epsilon * fabs(i->x0 + i->x1))
      break;

    if (evaluate_function(f, m, &fm)) return 1;

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
      fprintf(stderr, "i=%d: x=%.15f f(x)=%.15f\n", j, m, fm);
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
  int i, j, k;
  double *xi, *xj, *mij, *bi, *mii, d;

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
