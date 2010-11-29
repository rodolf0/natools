#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <baas/mod-arithmetic.h>

int main(int argc, char *argv[]) {
  uint64_t prime = atoll(argv[1]);
  int i, c = (argc > 2 ? atoi(argv[2]) : 100);

  // test 100 times if prime is prime
  for (i = 0; i < c; i++) {
    if (!test_prime(prime))
      break;
  }

  if (i == c)
    printf("%"PRIu64" is prime (err prob: %f)\n", prime, 1.0/(1L << c));
  else
    printf("%"PRIu64" is composite\n", prime);

  return 0;
}

/* modular division: x is a's inverse if a*x = 1 (mod N)
 * (a has inverse only if a is relatively prime to N (gcd(a, N) = 1))
 * we use the extended euclid's algorithm for dividing, x is the inverse */

int main_gcd(int argc, char *argv[]) {
  int a = atoi(argv[1]);
  int b = atoi(argv[2]);

  int x, y, d;
  extended_euclid(a, b, &x, &y, &d);

  printf("d=%d x=%d y=%d (such that a.x + b.y = d)\n", d, x, y);

  return 0;
}


/* vim: set sw=2 sts=2 : */
