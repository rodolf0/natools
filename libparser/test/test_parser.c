#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "../parser/parser.h"

#define EPSILON 1.0e-300

#define ASSERT_EVAL(s, d) \
  do { \
    parser_eval(p, (s), &r); \
    t = (d); \
    assert(fabs(r-t) < EPSILON); \
  } while (0)


void test_parser() {
  parser_t *p = parser_create();
  double t, r = 0.0;

  ASSERT_EVAL("1+1", 2.0);
  ASSERT_EVAL("34*9", 306.0);
  ASSERT_EVAL("b=3", 3.0);

  ASSERT_EVAL("34.2/92^-2.2*sin(0.23)",
              34.2/pow(92, -2.2)*sin(0.23));

  ASSERT_EVAL("(acos(pi/e^2)--23.3)^-1",
              pow((acos(M_PI/pow(M_E, 2)) - -23.3), -1));

  ASSERT_EVAL("max(pi, e) * min(pi, e) / 567.948e-2",
              M_PI * M_E / 567.948e-2);

  ASSERT_EVAL("-45.2*sum(0.1, 0.2, 0.3, 0.4)^log(atan(3.0))",
              -45.2*pow((0.1+0.2+0.3+0.4), log(atan(3.0))));

  ASSERT_EVAL("sin(1.4e-2)/tan(pi/3.0)^(0.5+0.45)",
              sin(1.4e-2)/pow(tan(M_PI/3.0), 0.5+0.45));

  parser_destroy(p);
}

#define ITERATIONS 10
int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    test_parser();
  }
  fprintf(stderr, "\n");

  return 0;
}

/* vim: set sw=2 sts=2 : */
