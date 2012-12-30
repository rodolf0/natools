#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "baas/bits.h"

void test_ints_to_bits(void) {
  char bits[33];
  int len;

  int32_t num1 = 123749123;
  int32_t num2 = 789453645;
  int32_t num3 = 475895234;
  int32_t num4 = 864502334;
  int32_t num5 = 324506094;

  len = int_to_bits(0, bits, sizeof(bits));
  assert(len == 1);
  assert(!strcmp(bits + 32 - len, "0"));

  len = int_to_bits(1, bits, sizeof(bits));
  assert(len == 1);
  assert(!strcmp(bits + 32 - len, "1"));

  len = int_to_bits(2, bits, sizeof(bits));
  assert(len == 2);
  assert(!strcmp(bits + 32 - len, "10"));

  len = int_to_bits(num1, bits, sizeof(bits));
  assert(!strcmp(bits + 32 - len, "111011000000100001100000011"));
  len = int_to_bits(num2, bits, sizeof(bits));
  assert(!strcmp(bits + 32 - len, "101111000011100001101101001101"));
  len = int_to_bits(num3, bits, sizeof(bits));
  assert(!strcmp(bits + 32 - len, "11100010111011001010111000010"));
  len = int_to_bits(num4, bits, sizeof(bits));
  assert(!strcmp(bits + 32 - len, "110011100001110100001000111110"));
  len = int_to_bits(num5, bits, sizeof(bits));
  assert(!strcmp(bits + 32 - len, "10011010101111001000111101110"));
}

void test_bitcount(void) {
  int n = random();
  assert(bitcount(n) == bitcount2(n));
}

#define ITERATIONS 100
int main(void) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    test_ints_to_bits();
    test_bitcount();
  }
  fprintf(stderr, "\n");

  return 0;
}

/* vim: set sw=2 sts=2 : */
