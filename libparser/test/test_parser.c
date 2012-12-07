#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "../parser/lexer.h"
#include "../parser/parser.h"

long double evaluate(const char *expr) {
  static parser_t *p = NULL;
  if (!p)
    p = parser_create();
  else if (!strcmp(expr, "shutdown")) {
    parser_destroy(p);
    return 0.0;
  }
  scanner_t *s = scanner_init(expr);
  long double r = 0.0;
  assert(parser_eval(p, s, &r) == 0);
  scanner_destroy(s);
  return r;
}


int main(int argc, char *argv[]) {
  assert(evaluate("23+45") == 68);
  assert(evaluate("23-45") == -22);
  assert(evaluate("-23") == -23);
  assert(evaluate("--5343") == 5343);
  assert(evaluate("56934.234") == 56934.234L);
  assert(evaluate("593063*56934.234") == 33765587618.742L);

  evaluate("shutdown");
  return 0;
}

/* vim: set sw=2 sts=2 : */
