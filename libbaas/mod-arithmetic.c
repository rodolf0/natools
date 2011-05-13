#include <stdlib.h>
#include "baas/mod-arithmetic.h"

int gcd(int a, int b) {
  if (b == 0)
    return a;
  else
    return gcd(b, a % b);
}


/* if d divides a and b, and d = xa + by =>  d = gcd(a, b) */
void extended_euclid(int a, int b, int *x, int *y, int *d) {
  int t;
  if (b == 0) {
    *x = 1; *y = 0; *d = a;
  } else {
    extended_euclid(b, a % b, x, y, d);
    t = *x;
    *x = *y;
    *y = t - a/b * *y;
  }
}


/* exponentiation modulo N */
uint64_t modexp(uint64_t x, uint64_t y, uint64_t N) {
  uint64_t z;
  if (y == 0)
    return 1;

  z = modexp(x, y/2, N);
  if (y & 1) // y is odd
    return ((x * z % N) * z) % N;
  else      // y is even
    return (z * z) % N;
}


// Use Fermat's little theorem to prove p is prime
// with proba of false positive <= 1/2 (ignoring Carmichael's nums)
int test_prime(int p) {
  uint64_t a = 1 + random() % (p-2); // random number 1 <-> p-1

  uint64_t z = modexp(a, p-1, p);

  //fprintf(stderr, "%llu^%llu (mod %llu) = %llu\n", a, p-1, p, z);

  if (z == 1) {
    // TODO: Rabin Miller Test to detect Carmichael numbers (test if composite)
    return 1;
  } else
    return 0;
}

/* vim: set sw=2 sts=2 : */
