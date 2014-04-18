#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "baas/bits.h"

void test_ints_to_bits(void) {
  struct {
    int32_t num;
    char    bits[33];
  } testv[] = {
    {0, "0"},
    {1, "1"},
    {2, "10"},
    {123749123, "111011000000100001100000011"},
    {789453645, "101111000011100001101101001101"},
    {475895234, "11100010111011001010111000010"},
    {864502334, "110011100001110100001000111110"},
    {324506094, "10011010101111001000111101110"},
  };

  for (size_t i = 0; i < sizeof(testv) / sizeof(testv[0]); ++i) {
    char bits[33];
    int len = int_to_bits(testv[i].num, bits, sizeof(bits));
    assert(!strcmp(bits + 32 - len, testv[i].bits));
  }
}

void test_bitcount(void) {
  int n = random();
  assert(bitcount(n) == bitcount2(n));
}

#define ITERATIONS 100
int main(void) {
  int i;
  test_ints_to_bits();
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    test_bitcount();
  }
  fprintf(stderr, "\n");

  return 0;
}

/* vim: set sw=2 sts=2 : */
