#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "baas/bignum.h"

static const char digit[] = "0123456789";

void random_bigint(char *n, size_t d) {
  size_t len = 2 + random() % (d-2);
  for (size_t i = 0; i < len; ++i)
    n[i] = digit[random() % 9];
  n[len-1] = '\0';
}

void test_big_add(void) {
  char n1[18], n2[18];
  random_bigint(n1, 18);
  random_bigint(n2, 18);
  char *r = big_add(n1, n2);
  assert(strtoull(r, NULL, 10) ==
         strtoull(n1, NULL, 10) + strtoull(n2, NULL, 10));
  free(r);
}

void test_big_mul(void) {
  char n1[10], n2[8];
  random_bigint(n1, 10);
  random_bigint(n2, 8);
  char *r = big_mul(n1, n2);
  assert(strtoull(r, NULL, 10) ==
         strtoull(n1, NULL, 10) * strtoull(n2, NULL, 10));
  free(r);
}

void test_bignum(void) {
  const char *num1 = "98143278943523452345789012347890543257890123473890543257"
                     "8902341789054325789012347890234578902345";
  const char *num2 = "98723450823475871234089754320987234897052435789013240987"
                     "543209871234780234508971234";
  char *result;

  result = big_add(num1, num2);
  assert(!strcmp(result, "09814327894353332469087135993501395223332222219738"
                         "02485014691355030041868998883582670469087873579"));
  free(result);

  result = big_mul(num1, num2);
  assert(!strcmp(result, "968904317243561250527515505659747936287024828385"
                         "727972083025367497594773491424682907460548592048"
                         "978383210171914895096073300883425357575387395841"
                         "73736227468144009012422126900143730"));
  free(result);
}

#define ITERATIONS 10000
int main(void) {
  int i;
  test_bignum();
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    test_big_add();
    test_big_mul();
  }
  fprintf(stderr, "\n");

  return 0;
}

/* vim: set sw=2 sts=2 : */
