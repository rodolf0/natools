#include <stdio.h>
#include <assert.h>

#include "../parser/lexer.h"
#include "../parser/parser.h"

int main(int argc, char *argv[]) {
  /*scanner_t *s = scanner_init("2+2**3/2");*/
  scanner_t *s = scanner_init(argv[1]);
  parser_t *p = parser_create();
  expr_t *e = parser_compile(p, s);
  long double r;
  evaluate_expression(e, &r, NULL, NULL);
  fprintf(stderr, "%.15Lg\n", r);
  parser_destroy(p);
  scanner_destroy(s);
  return 0;
}

/* vim: set sw=2 sts=2 : */
