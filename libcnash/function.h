#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <cnash/parser.h>

typedef struct _function {
  /* the function f(x) */
  char *function;
  /* parser used to evaluate the function */
  parser_t *parser;
} function_t;

function_t *function_create(const char *func);
void function_destroy(function_t *f);

int evaluate_function(function_t *f, double x0, double *f0);

#endif /* _FUNCTION_H_ */

/* vim: set sw=2 sts=2 : */
