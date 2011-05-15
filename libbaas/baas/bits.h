#ifndef _BITS_H_
#define _BITS_H_

/* int_to_bits: convert integer to bit-string */
int int_to_bits(int i, char *bits, int length);

/* bitcount: count how many 1's in binary representation of number */
int bitcount(int i);
int bitcount2(int i);

#endif

/* vim: set sw=2 sts=2 : */
