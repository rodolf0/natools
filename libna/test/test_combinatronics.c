#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "../na/combinatorics.h"


void test_factorial() {
  assert(factorial(0) == 1);
  assert(factorial(1) == 1);
  assert(factorial(5) == 120);
  assert(factorial(10) == 3628800);
}

void test_combinations() {
  assert(k_combinations(5, 3) == 10);
  assert(k_combinations(7, 2) == 21);
  assert(k_combinations(50, 4) == 230300);
  assert(k_combinations(19, 8) == 75582);
}

void test_permutations() {
  assert(k_permutations(5, 3) == 60);
  assert(k_permutations(7, 2) == 42);
  assert(k_permutations(50, 4) == 5527200);
  assert(k_permutations(19, 8) == 3047466240);
}

int main(int argc, char *argv[]) {
  test_factorial();
  test_combinations();
  test_permutations();
  return 0;
}

/* vim: set sw=2 sts=2 : */
