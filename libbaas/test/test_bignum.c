#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../baas/bignum.h"

static const char digit[] = "0123456789";

void test_bignum() {
  char num1[8], num2[8], *result;
  int i, nlen;

  nlen = random() % 8;
  for (i = 0; i < nlen; i++)
    num1[i] = digit[random() % 9];
  num1[nlen-1] = '\0';

  nlen = random() % 8;
  for (i = 0; i < nlen; i++)
    num2[i] = digit[random() % 9];
  num2[nlen-1] = '\0';

  result = big_add(num1, num2);
  assert(atoi(result) == atoi(num1) + atoi(num2));
  free(result);

  result = big_mul(num1, num2);
  assert(atoi(result) == atoi(num1) * atoi(num2));
  free(result);
}

void test_bignum2() {
  char *result;
  const char *num1 = "98143278943523452345789012347890543257890123473890543257"
                     "8902341789054325789012347890234578902345";
  const char *num2 = "98723450823475871234089754320987234897052435789013240987"
                     "543209871234780234508971234";

  result = big_add(num1, num2);
  assert(0==strcmp(result, "09814327894353332469087135993501395223332222219738"
                           "02485014691355030041868998883582670469087873579"));
  free(result);

  result = big_mul(num1, num2);
  assert(0 == strcmp(result, "968904317243561250527515505659747936287024828385"
                             "727972083025367497594773491424682907460548592048"
                             "978383210171914895096073300883425357575387395841"
                             "73736227468144009012422126900143730"));
  free(result);
}

#define ITERATIONS 10000
int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    test_bignum();
    test_bignum2();
  }
  fprintf(stderr, "\n");

  return 0;
}

/* vim: set sw=2 sts=2 : */
