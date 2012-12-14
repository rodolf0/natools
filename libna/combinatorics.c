#include <math.h>

#include "na/combinatorics.h"

long int factorial(int n) {
  return lroundl(tgamma(n + 1));
}

/*                n!
 * C(n, k) = ------------
 *            k!(n - k)!
 * combinations of k-elements from a set of size n
 * in combinations the order is not important
 * */
long int k_combinations(int n, int k) {
  return lroundl(expl(lgammal(n+1) - lgammal(k+1) - lgammal(n-k+1)));
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
  return lroundl(expl(lgammal(n+1) - lgammal(n-k+1)));
}

/* n^k */
long int k_permutations_rep(int n, int k) {
  return lroundl(powl(n, k));
}

/* vim: set sw=2 sts=2 : */
