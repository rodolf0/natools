#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <cnash/parser.h>
#include "root_search.h"

#define c_epsilon "1.0e-15"
#define c_maxiter "1000"

roots_t *roots_init(const char *f, double x0, double x1,
                    double epsilon, double max_iterations) {

  size_t f_len = strlen(f);
  roots_t *r = (roots_t*)malloc(sizeof(roots_t));

  r->parser = parser_create();

  r->function = (char*)malloc(f_len + 1);
  strncpy(r->function, f, f_len);
  r->function[f_len] = '\0';
  
  r->x1 = x1;
  r->x0 = x0;
  r->epsilon = epsilon;
  r->max_iterations = max_iterations;
  r->verbose = 0;

  return r;
}

void roots_destroy(roots_t *rs) {
  if (!rs)
    return;
  if (rs->function)
    free(rs->function);
  if (rs->parser)
    parser_destroy(rs->parser);
  free(rs);
}

/* evaluates f(x) at x = x0 */
static int eval_func_at(roots_t *rs, double x0, double *r) {
  char asig_buf[256];
  
  /* load x=x0 into the parser */
  snprintf(asig_buf, sizeof(asig_buf), "x=%.15e", x0);
  if (parser_eval(rs->parser, asig_buf, r))
    return 1;
  /* evaluate f(x) */
  if (parser_eval(rs->parser, rs->function, r))
    return 2;
  return 0;
}

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
int root_secant(roots_t *rs) {
  double d, f1, f0;
  int i;

  /* evaluate function at x0 */
  if (eval_func_at(rs, rs->x0, &f0)) return 1;
  /* evaluate function at x1 */
  if (eval_func_at(rs, rs->x1, &f1)) return 1;

  for (i = 0; i < rs->max_iterations; i++) {

    /* calculate next aproximation */
    d = f1 * (rs->x1 - rs->x0) / (f1 - f0);
    /* check stop condition */
    if (fabs(d) < rs->epsilon)
      break;

    /* update secant points */
    rs->x0 = rs->x1; f0 = f1;
    rs->x1 = rs->x1 - d;
    if (eval_func_at(rs, rs->x1, &f1)) return 1;

    if (rs->verbose)
      fprintf(stderr, "i=%d: x=%.15f f(x)=%.15f d=%.15f\n", i, rs->x1, f1, d);
  }

  rs->root = rs->x1;
  return 0;
}

int root_newton(roots_t *rs) {
  double f0, f1, df0_dx, d;
  const double dx = 1.0e-7; /* infinitesim */
  int i;

  for (i = 0; i < rs->max_iterations; i++) {

    /* evaluate function at x0 */
    if (eval_func_at(rs, rs->x0, &f0)) return 1;
    /* evaluate f'(x) at x0 */
    if (eval_func_at(rs, rs->x0 + dx, &f1)) return 1;
    df0_dx = (f1 - f0) / dx;

    if (fabs(d = f0 / df0_dx) < rs->epsilon)
      break;

    rs->x0 = rs->x0 - d;

    if (rs->verbose)
      fprintf(stderr, "i=%d: x=%.15f f(x)=%.15f d=%.15f\n", i, rs->x0, f0, d);
  }

  rs->root = rs->x0;
  return 0;
}

/* x0 and x1 are the endpoints for the search interval */
int root_bisection(roots_t *rs) {
  double m, fm, f1, f0;
  int i;

  if (eval_func_at(rs, rs->x0, &f0)) return 1;
  if (eval_func_at(rs, rs->x1, &f1)) return 1;

  /* first assure f(x0) and f(x1) have opposite signs */
  if (f0 < 0.0 && f1 > 0.0) {
    ; // do nothing x0 and x1 are in order
  } else if (f0 > 0.0 && f1 < 0.0) {
    m = rs->x0; rs->x0 = rs->x1; rs->x1 = m;
    m = f0; f0 = f1; f1 = m;
  } else {
    fprintf(stderr, "f(x0) * f(x1) !< 0\n");
    return 3;
  }

  for (i = 0; i < rs->max_iterations; i++) {

    /* evaluate function at midpoint */
    m = (rs->x0 + rs->x1) / 2.0;
    if (eval_func_at(rs, m, &fm)) return 1;

    /* check stop conditions */
    if (rs->x0 == m || rs->x1 == m || 
        fabs(rs->x0 - rs->x1) < rs->epsilon)
      break;

    /* update endpoints */
    if (fm < 0.0) {
      rs->x0 = m; f0 = fm;
    } else {
      rs->x1 = m; f1 = fm;
    }

    if (rs->verbose)
      fprintf(stderr, "i=%d: x=%.15f f(x0)=%.15f f(x1)=%.15f\n",
              i, (rs->x1 + rs->x0) / 2.0, f0, f1);
  }

  rs->root = (rs->x0 + rs->x1) / 2.0;
  return 0;
}

/* x0 and x1 are the endpoints for the search interval
 * Aproximate the the root of the function by finding
 * the root of the secant.
 * 
 * down-weighting is applied to improve method and
 * make it suprlinear convergent.
 * */
int root_regulafalsi(roots_t *rs) {
  double m, fm, f1, f0;
  int i, side = 0;

  if (eval_func_at(rs, rs->x0, &f0)) return 1;
  if (eval_func_at(rs, rs->x1, &f1)) return 1;

  /* first assure f(x0) and f(x1) have opposite signs */
  if (f0 < 0.0 && f1 > 0.0) {
    ; // do nothing x0 and x1 are in order
  } else if (f0 > 0.0 && f1 < 0.0) {
    m = rs->x0; rs->x0 = rs->x1; rs->x1 = m;
    m = f0; f0 = f1; f1 = m;
  } else {
    fprintf(stderr, "f(x0) * f(x1) !< 0\n");
    return 3;
  }

  for (i = 0; i < rs->max_iterations; i++) {

    m = (f1 * rs->x0 - f0 * rs->x1) / (f1 - f0);
    /* check stop conditions */
    if (fabs(rs->x0 - rs->x1) < 
        rs->epsilon * fabs(rs->x0 + rs->x1))
      break;

    if (eval_func_at(rs, m, &fm)) return 1;

    /* update endpoints */
    if (fm < 0.0) {
      if (side == -1) f1 /= 2.0;
      rs->x0 = m; f0 = fm;
      side = -1;
    } else if (fm > 0.0) {
      /* down-weight x0 side to avoid 
       * linear convergence because of choosing
       * always the same side */
      if (side == 1) f0 /= 2.0;
      rs->x1 = m; f1 = fm;
      side = 1;
    } else
      break;

    if (rs->verbose)
      fprintf(stderr, "i=%d: x=%.15f f(x)=%.15f\n", i, m, fm);
  }

  rs->root = m;
  return 0;
}

/* vim: set sw=2 sts=2 : */
