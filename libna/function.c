#include <stdlib.h>
#include <string.h>

#include "na/function.h"
#include "parser/parser.h"

struct function_t {
  expr_t *expr;
  hashtbl_t *vars;
};

function_t * function_create(const char *func) {
  function_t *f = zmalloc(sizeof(function_t));
  f->expr = parser_compile_str(func);
  f->vars = hashtbl_init(free, NULL);
  return f;
}

void function_destroy(function_t *f) {
  if (!f)
    return;
  hashtbl_destroy(f->vars);
  parser_destroy_expr(f->expr);
  free(f);
}

long double function_eval(function_t *f, long double x0) {
  long double *x = hashtbl_get(f->vars, "x");
  if (!x) {
    x = zmalloc(sizeof(long double));
    *x = x0;
    hashtbl_insert(f->vars, "x", x);
  } else
    *x = x0;

  parser_eval(f->expr, &x0, f->vars);
  return x0;
}

/* vim: set sw=2 sts=2 : */
