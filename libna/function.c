#include <stdlib.h>
#include <string.h>

#include "parser/lexer.h"
#include "parser/parser.h"

typedef struct _function_t {
  parser_t *p;
  char *f;
} function_t;

function_t * function_create(const char *func) {
  size_t len = strlen(func);
  function_t *f = (function_t*)malloc(sizeof(function_t) + len + 1);
  f->p = parser_create();
  f->f = (char*)f + sizeof(function_t);
  strncpy(f->f, func, len);
  f->f[len] = '\0';
  return f;
}

void function_destroy(function_t *f) {
  if (!f)
    return;
  if (f->p)
    parser_destroy(f->p);
  free(f);
}

long double function_eval(function_t *f, long double x0) {
  if (!f) {
#ifdef _VERBOSE_
    fprintf(stderr, "func eval: Null function evaluation\n");
#endif
    return 0.0;
  }
  scanner_t *s = scanner_init(f->f);
  parser_setvar(f->p, "x", x0);
  parser_eval(f->p, s, &x0);
  scanner_destroy(s);
  return x0;
}

/* vim: set sw=2 sts=2 : */
