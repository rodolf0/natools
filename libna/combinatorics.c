#include <math.h>

#include "na/combinatorics.h"

long int factorial(int n) {
  long int r = 1, i;
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
long int k_combinations(int n, int k) {
  long double r = 1.0;
  long int i;
  for (i=0, r = 1.0; i < k; i++)
    r *= (long double)(n - i) / (long double)(k - i);
  return (int)roundl(r);
}

long int k_combinations_rep(int n, int k) {
  return k_combinations(n + k - 1, n - 1);
}

/*               n!
 * P(n, k) = ---------- = k! * C(n, k)
 *            (n - k)!
 * permutation of k selected elements from a set of n
 * in permutations order of selection is important
 * */
long int k_permutations(int n, int k) {
  long int i, r;
  for (i = 0, r = 1; i < k; i++)
    r *= n - i;
  return r;
}

/* n^k */
long int k_permutations_rep(int n, int k) {
  long int i, r;
  for (i = 0, r = 1; i < k; i++)
    r *= n;
  return r;
}

/* vim: set sw=2 sts=2 : */
