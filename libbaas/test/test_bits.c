#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cnash/bits.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <number>\n", argv[0]);
    return 1;
  }

  int input = atoi(argv[1]);
  char bits[64];

  int_to_bits(input, bits, sizeof(bits));

  char *pbit;
  if ((pbit = strchr(bits, '1')) != NULL)
    printf("%s", pbit);
  else
    printf("0");

  printf(": %d bits on\n", bitcount(input));

  return 0;
}

/* vim: set sw=2 sts=2 : */
