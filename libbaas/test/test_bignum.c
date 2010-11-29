#include <stdio.h>
#include <stdlib.h>
#include <baas/bignum.h>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <number> <number>\n", argv[0]);
    return 1;
  }

  char *add = big_add(argv[1], argv[2]);
  char *mul = big_mul(argv[1], argv[2]);

  printf("+: %s\n", add);
  printf("*: %s\n", mul);

  free(add);
  free(mul);

  return 0;
}

/* vim: set sw=2 sts=2 : */
