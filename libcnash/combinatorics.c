#include "combinatorics.h"

int factorial(int n) {
  int r = 1, i;

  for (i = 1; i <= n; i++)
    r *= i;

  return r;
}

/*                n!
 * C(n, k) = ------------
 *            k!(n - k)!
 * combinations of k-elements from a set of size n
 * in combinations the order is not important
 * */
int k_combinations(int n, int k) {
  double r = 1.0;
  int i;

  for (i=0, r = 1.0; i < k; i++)
    r *= (double)(n - i) / (double)(k - i);

  return (int)r;
}

int k_combinations_rep(int n, int k) {
  return k_combinations(n + k - 1, n - 1);
}

/*               n!
 * P(n, k) = ---------- = k! * C(n, k)
 *            (n - k)!
 * permutation of k selected elements from a set of n
 * in permutations order of selection is important
 * */
int k_permutations(int n, int k) {
  int i, r;

  for (i = 0, r = 1; i < k; i++)
    r *= n - i;

  return r;
}

/* n^k */
int k_permutations_rep(int n, int k) {
  int i, r;

  for (i = 0, r = 1; i < k; i++)
    r *= n;

  return r;
}

/* vim: set sw=2 sts=2 : */
