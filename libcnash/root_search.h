#ifndef _ROOT_SEARCH_H_
#define _ROOT_SEARCH_H_

#include <cnash/parser.h>

typedef struct _roots {
  /* the function to find the root for */
  char *function;
  /* x0 and x1 are initial points 
   * some methods use both some just one */
  double x0, x1;
  /* the actual result */
  double root;

  /* epsilon: max error before search stops */
  double epsilon;
  /* maximum iterations before giving up */
  int max_iterations;

  parser_t *parser;
  /* output aproximations */
  int verbose;
} roots_t;

roots_t *roots_init(const char *f, double x0, double x1,
                    double epsilon, double max_iterations);

void roots_destroy(roots_t *rs);

int root_secant(roots_t *rs);
int root_newton(roots_t *rs);
int root_bisection(roots_t *rs);
int root_regulafalsi(roots_t *rs);

#endif /* _ROOT_SEARCH_H_ */

/* vim: set sw=2 sts=2 : */
