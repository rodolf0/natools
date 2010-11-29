#ifndef _SORT_CMP_H_
#define _SORT_CMP_H_

#include <string.h>

int int_cmp(const int *a, const int *b) {
  return (*a == *b ? 0 : *a < *b ? -1 : 1);
}

/* mostly usefull for argv[...] */
int str_cmp(char **a, char **b) {
  return strcmp(*a, *b);
}

#endif

/* vim: set sw=2 sts=2 : */
