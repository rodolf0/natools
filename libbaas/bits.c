#include <string.h>

/* int_to_bits: convert integer to bit-string */
void int_to_bits(int i, char *bits, int length) {
  int power, tmp;
  /* set all bits to 0 */
  memset(bits, '0', length);
  bits[length-1] = '\0';
  /* calc powers of 2 */
  while (i > 0) {
    power = 0; tmp = i;
    while (tmp > 1) {
      tmp >>= 1;
      power++;
    }
    /* don't use the \0 pos */
    bits[length-2 - power] = '1';
    i -= 1L << power;
  }
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
