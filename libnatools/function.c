#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser/lexer.h"
#include "parser/parser.h"
#include "natools/function.h"

function_t *function_create(const char *func) {
  function_t *f = (function_t*)malloc(sizeof(function_t));
  size_t f_len = strlen(func);

  f->function = (char*)malloc(f_len + 1);
  strncpy(f->function, func, f_len);
  f->function[f_len] = '\0';
  f->parser = parser_create();

  return f;
}

void function_destroy(function_t *f) {
  if (!f)
    return;
  if (f->function)
    free(f->function);
  if (f->parser)
    parser_destroy(f->parser);
  free(f);
}

/* evaluates f(x) at x = x0
 * uppon return x0 holds the result */
int evaluate_function(function_t *f, long double x0, long double *f0) {
  if (!f) return 1;
  char asig_buf[256];
  /* load x=x0 into the parser and evaluate f(x0) */
  snprintf(asig_buf, sizeof(asig_buf), "x=%.20Lg", x0);
  scanner_t *s = scanner_init(asig_buf);
  if (parser_eval(f->parser, s, f0)) return 2;
  scanner_destroy(s);
  s = scanner_init(f->function);
  if (parser_eval(f->parser, s, f0)) return 3;
  scanner_destroy(s);
  return 0;
}

/* vim: set sw=2 sts=2 : */
