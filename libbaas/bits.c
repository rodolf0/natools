#include <string.h>
#include "baas/bits.h"

/* int_to_bits: convert integer to bit-string return valid length */
int int_to_bits(int i, char *bits, int length) {
  int power = 0, tmp;
  /* set all bits to 0 */
  memset(bits, '0', length);
  bits[length-1] = '\0';
  /* calc powers of 2 */
  while (i > 0) {
    power = 0; tmp = i;
    /* find the next turned on bit */
    while (tmp > 1) {
      tmp >>= 1;
      power++;
    }
    /* don't use the \0 pos */
    bits[length-2 - power] = '1';
    i -= 1L << power;
  }
  char *leading1 = strchr(bits, '1');
  return  (leading1 ? bits + length - 1 - leading1 : 1);
}


/* count how many 1's in binary representation of number */
int bitcount(int i) {
  int count = 0;
  while (i > 0) {
    count++;
    i &= i - 1; // clear the lsb
  }
  return count;
}


/* slower version */
int bitcount2(int i) {
  int count = 0;
  while (i > 0) {
    if (i & 1)
      count++;
    i >>= 1;
  }
  return count;
}

/* vim: set sw=2 sts=2 : */
