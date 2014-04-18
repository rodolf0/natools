#include <stdlib.h>
#include <string.h>
#include "baas/bignum.h"

static const char digit[] = "0123456789";


/* adds to numbers with no presicion limit */
char * big_add(const char *x, const char *y) {
  int xlen = strlen(x);
  int ylen = strlen(y);
  // add a digit for carry
  int rlen = (xlen > ylen ? xlen : ylen) + 1;
  char *ret = (char*)zmalloc(rlen + 1);

  int i, r, carry;
  // single digit adition
  for (i = 1, carry = 0; i <= rlen; i++) {
    r = carry;
    if (i <= xlen) r += x[xlen - i] - '0'; // convert to int
    if (i <= ylen) r += y[ylen - i] - '0';
    carry = r / 10; r %= 10; // single digit

    ret[rlen - i] = digit[r];
  }
  ret[rlen] = '\0';
  return ret;
}


/* multiplies to numbers with no presicion limit */
char * big_mul(const char *x, const char *y) {
  int xlen = strlen(x);
  int ylen = strlen(y);
  // temporary result for each step
  int alen = xlen + 1; // add place for carry digit
  char *a = (char*)zmalloc(alen + 1);
  // add a digit for carry
  int rlen = xlen + ylen;
  char *ret = (char*)zmalloc(rlen + 1);
  int r, i, j, carry;
  // set the initial result to 0 since we'll be acumulating
  memset(ret, '0', rlen);

  for (i = 1; i <= ylen; i++) {
    // multiply each digit of y by each digit of x
    for (j = 1, carry = 0; j <= xlen; j++) {
      r = (x[xlen - j] - '0') * (y[ylen - i] - '0') + carry;
      carry = r / 10; r %= 10; // single digit
      a[alen - j] = digit[r];
    }
    a[alen - j] = digit[carry]; // a has space for one more digit than x
    // accumulate the temporary result to the final one
    for (j = 1, carry = 0; j <= alen; j++) {
      r = carry + (a[alen - j] - '0') + (ret[rlen - j - i+1] - '0');
      carry = r / 10; r %= 10; // single digit
      ret[rlen - j - i+1] = digit[r];
    }
    if (i != ylen) // las round adding to 0
      ret[rlen - j - i+1] = digit[carry];
  }

  free(a);
  ret[rlen] = '\0';
  return ret;
}

/* vim: set sw=2 sts=2 : */
