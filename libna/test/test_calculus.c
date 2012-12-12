#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../na/function.h"
#include "../na/calculus.h"

#define EPSILON 1.0e-5
#define ASSERT_EQ(x, y) assert(fabsl((x)-(y)) < EPSILON)

void test_derivates() {
  function_t *f = function_create("x**2");
  ASSERT_EQ(derivate_1(f, 0.2), 0.4);
  ASSERT_EQ(derivate_1(f, 0.0), 0.0);
  ASSERT_EQ(derivate_2(f, 0.0), 2.0);
  function_destroy(f);

  f = function_create("cos(x) - x**3");
  ASSERT_EQ(derivate_1(f, 5.0), -75.0-sinl(5.0));
  ASSERT_EQ(derivate_1(f, 0.0), 0.0);
  ASSERT_EQ(derivate_2(f, 0.0), -1.0);
  ASSERT_EQ(derivate_2(f, 3.75), -21.6794);
  ASSERT_EQ(derivate(f, 3, -34.2), -6.34995);
  ASSERT_EQ(derivate(f, 3, -3.2), -5.94163);
  function_destroy(f);

  f = function_create("2**x - log(x)");
  ASSERT_EQ(derivate_1(f, 1.5), 1.29385);
  ASSERT_EQ(derivate_1(f, 3.2), 6.05724);
  ASSERT_EQ(derivate_2(f, 3.2), 4.51282);
  ASSERT_EQ(derivate_2(f, 5.0), 15.41446);
  function_destroy(f);
}

void test_arclength() {
  function_t *f = function_create("2**x - log(x)");
  ASSERT_EQ(arc_length(f, 0.5, 2.3), 3.0663188081);
  function_destroy(f);

  f = function_create("1/x + sin(x)**0.3");
  ASSERT_EQ(arc_length(f, 0.5, 2.3), 2.4417982309);
  function_destroy(f);

  f = function_create("3.5*x**4 - 30.3*x**3 + 7.2*x**2 - 3.4*x + 32.0");
  ASSERT_EQ(arc_length(f, -5, 7), 8109.52041086);
  function_destroy(f);

  f = function_create("3.5*x**4 - 7.2*x**2 - 3.4*x - 32.0");
  ASSERT_EQ(arc_length(f, -1.5, 0.73), 14.196939434);
  function_destroy(f);
}

int main(int argc, char *argv[]) {
  test_derivates();
  test_arclength();
  return 0;
}

/* vim: set sw=2 sts=2 : */
