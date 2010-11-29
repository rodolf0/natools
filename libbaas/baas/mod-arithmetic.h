#ifndef _MOD_ARITHMETIC_H_
#define _MOD_ARITHMETIC_H_

#include <stdint.h>

int gcd(int a, int b);

/* if d divides a and b, and d = xa + by =>  d = gcd(a, b) */
void extended_euclid(int a, int b, int *x, int *y, int *d);

uint64_t modexp(uint64_t x, uint64_t y, uint64_t N);

int test_prime(int p);

#endif /* _MOD_ARITHMETIC_H_ */

/* vim: set sw=2 sts=2 : */
