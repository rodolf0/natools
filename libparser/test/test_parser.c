#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "parser/parser.h"
#include "baas/hashtbl.h"

static hashtbl_t *vars = NULL;

long double evaluate(const char *expr) {
  expr_t *e = NULL;
  if (!(e = parser_compile_str(expr)))
    abort();

  long double r = 0.0; int error = 0;
  if ((error = parser_eval(e, &r, vars)) != 0) {
    fprintf(stderr, "[%s] failed with error: %d\n", expr, error);
    abort();
  }
  parser_destroy_expr(e);
  return r;
}

#define EPSILON 1.0e-10
#define ASSERT_EQ(x, y) assert(fabsl((x)-(y)) < EPSILON)
#define ASSERT_EPS(x, y, eps) assert(fabsl((x)-(y)) < (eps))


void check_operators(void) {
  ASSERT_EQ(evaluate("23+45"), 68);
  ASSERT_EQ(evaluate("23-45"), -22);
  ASSERT_EQ(evaluate("-23"), -23);
  ASSERT_EQ(evaluate("--5343"), 5343);
  ASSERT_EQ(evaluate("56934.234"), 56934.234L);
  ASSERT_EQ(evaluate("23.41 * 34e+2"), 79594);
  ASSERT_EQ(evaluate("98 / 0.4235"), 231.40495867768);
  ASSERT_EQ(evaluate("9%4"), 1);
  ASSERT_EQ(evaluate("23489 % 234.23"), 66);
  ASSERT_EQ(evaluate("3**3"), 27);
  ASSERT_EQ(evaluate("16**2"), 256);
  ASSERT_EQ(evaluate("144**0.5"), 12);

  ASSERT_EQ(evaluate("64 >> 2"), 16);
  ASSERT_EQ(evaluate("64 >> 2.2"), 16);
  ASSERT_EQ(evaluate("1 << 5"), 32);
  ASSERT_EQ(evaluate("8 & 9"), 8);
  ASSERT_EQ(evaluate("64 & 63"), 0);
  ASSERT_EQ(evaluate("7 & 5"), 5);
  ASSERT_EQ(evaluate("7 | 5"), 7);
  ASSERT_EQ(evaluate("7 | 8"), 15);
  ASSERT_EQ(evaluate("8 ^ 7"), 15);
  ASSERT_EQ(evaluate("10 ^ 11"), 1);
  ASSERT_EQ(evaluate("~0"), -1);
  ASSERT_EQ(evaluate("~1"), -2);

  ASSERT_EQ(evaluate("not 0"), 1);
  ASSERT_EQ(evaluate("0 and 3"), 0);
  ASSERT_EQ(evaluate("0 or 3"), 1);
  ASSERT_EQ(evaluate("true"), 1);
  ASSERT_EQ(evaluate("false"), 0);

  ASSERT_EQ(evaluate("3.1415 == 31415e-4"), 1);
  ASSERT_EQ(evaluate("3.1415 != 31415e-2"), 1);
  ASSERT_EQ(evaluate("3.1415 > 31415e-2"), 0);
  ASSERT_EQ(evaluate("3.1415 < 31415e2"), 1);
  ASSERT_EQ(evaluate("3.1415 >= 31415e2"), 0);
  ASSERT_EQ(evaluate("3.1415 <= 31415e2"), 1);

  ASSERT_EQ(evaluate("(75.34)"), 75.34);
  ASSERT_EQ(evaluate("-(75.34)"), -75.34);
  ASSERT_EQ(evaluate("3*(75.34-4)"), 214.02);
  ASSERT_EQ(evaluate("2**(3-1)"), 4);
}

void check_functions(void) {
  long double *a = (long double*)malloc(sizeof(long double));
  *a = 46;
  hashtbl_insert(vars, "a", a);

  ASSERT_EQ(evaluate("max(10, 20, 12, 15)"), 20);
  ASSERT_EQ(evaluate("min(11, 21, 15, 25)"), 11);
  ASSERT_EQ(evaluate("sum(1, 2, 3, 4, 5, 6)"), 21);
  ASSERT_EQ(evaluate("avg(3.4, 4e-2, 3.5, a)"), 13.235);
  ASSERT_EQ(evaluate("abs(-34)"), 34);

  *a = evaluate("random()");
  ASSERT_EQ(evaluate("cos(a)**2 + sin(a)**2"), 1.0);
  ASSERT_EQ(evaluate("sin(phi)/cos(phi) - tan(phi)"), 0.0);
  ASSERT_EQ(evaluate("1 + tan(a)**2 - 1/cos(a)**2"), 0.0);

  ASSERT_EQ(evaluate("atan2(a, phi) == atan(a/phi)"), 1);
  ASSERT_EQ(evaluate("acos(cos(phi)) - phi"), 0.0);
  ASSERT_EQ(evaluate("asin(sin(phi/4)) == phi/4"), 1);

  ASSERT_EQ(evaluate("log(exp(3))"), 3);
  ASSERT_EPS(evaluate("log(234 * 4234) - log(234) - log(4234)"), 0.0, 1.0e-5);

  ASSERT_EQ(roundl(evaluate("gamma(16)")), 1307674368000);
}

void check_precedence(void) {
  ASSERT_EQ(evaluate("3 + 4 * 5"), 23);
  ASSERT_EQ(evaluate("(3 + 4) * 5"), 35);

  ASSERT_EQ(evaluate("3*5**2"), 75);
  ASSERT_EQ(evaluate("(3 * 5)**2"), 225);
  ASSERT_EQ(evaluate("-3**2"), 9);

  ASSERT_EQ(evaluate("3 * 7 % 3"), 0);
  ASSERT_EQ(evaluate("3 * (7 % 3)"), 3);

  ASSERT_EQ(evaluate("32 >> 3 & 6"), 4);
  ASSERT_EQ(evaluate("32 >> (3 & 6)"), 8);

  ASSERT_EQ(evaluate("4 << (7 ^ 5)"), 16);

  ASSERT_EQ(evaluate("4 | 5 ^ 3 & 2"), 7);
  ASSERT_EQ(evaluate("4 | (5 ^ 3) & 2"), 6);

  ASSERT_EQ(evaluate("false and false or true"), 1);
  ASSERT_EQ(evaluate("false and (false or true)"), 0);

  ASSERT_EQ(evaluate("1 < 2 < 3"), 1);

  ASSERT_EQ(evaluate("max(3, 4) - -min(-3, 4)"), 1);
}

void check_longer(void) {
  ASSERT_EQ(evaluate("5.23e+3**4**-2 + avg(34>>2, phi < pi, max(phi, pi, 3.2))"), 5.774346129827);
  long double *x = (long double*)malloc(sizeof(long double));
  hashtbl_insert(vars, "x", x);
  *x = 3.0;
  ASSERT_EQ(evaluate("e**tan(x)/(1+x**2)*sin((1+log(x)**2)**0.5)"), 0.0864000589547301);
  *x = 17.25;
  ASSERT_EPS(evaluate("e**tan(x)/(1+x**2)*sin((1+log(x)**2)**0.5)"), 514696792827.659, 1.0e-3);
}

int main(void) {
  vars = hashtbl_init(free, NULL);
  /*fprintf(stderr, "%.15Lg\n", evaluate(argv[1]));*/
  check_operators();
  check_functions();
  check_precedence();
  check_longer();
  hashtbl_destroy(vars);
  return 0;
}

/* vim: set sw=2 sts=2 : */
