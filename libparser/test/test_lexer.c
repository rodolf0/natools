#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../parser/lexer.h"


void test_numbers() {
  token_t *t;
  scanner_t *s = scanner_init("123 123.45 123e2 987E+3 432.324e3 245.1e-14");
  /* TODO: these results are not super accurate */
  const double sum = 1431870.45, prod = 195330978.95594069;
  double mysum = 0.0, myprod = 1.0;

  while ((t = lexer_nextitem(s))->lexcomp != tokStackEmpty) {
    double x = atof(t->lexem);
    mysum += x;
    myprod *= x;
    free(t);
  }
  free(t);
  assert(sum == mysum);
  assert(prod == myprod);
  scanner_destroy(s);
}


void test_lexer() {
  token_t *t;
  char buf[1024];

  token_t tokens[] = {
    { tokNumber        ,  "123" },
    { tokPower         ,  "**" },
    { tokNumber        ,  "123.45" },
    { tokMinus         ,  "-" },
    { tokFalse         ,  "false" },
    { tokTimes         ,  "*" },
    { tokNumber        ,  "245.1e-14" },
    { tokDivide        ,  "/" },
    { tokAnd           ,  "and" },
    { tokOParen        ,  "(" },
    { tokBitXor        ,  "^" },
    { tokId            ,  "a" },
    { tokBitAnd        ,  "&" },
    { tokAsign         ,  "=" },
    { tokNot           ,  "not" },
    { tokModulo        ,  "%" },
    { tokFunction      ,  "sin(" },
    { tokBitNot        ,  "~" },
    { tokId            ,  "pi" },
    { tokCParen        ,  ")" },
    { tokTrue          ,  "true" },
    { tokMinus         ,  "-" },
    { tokOr            ,  "or" },
    { tokBitOr         ,  "|" },
    { tokOParen        ,  "(" },
    { tokMinus         ,  "-" },
    { tokNumber        ,  "43" },
    { tokPlus          ,  "+" },
    { tokNumber        ,  "23" },
    { tokComma         ,  "," },
    { tokCParen        ,  ")" },
    { tokRShift        ,  ">>" },
    { tokGt            ,  ">" },
    { tokLt            ,  "<" },
    { tokId            ,  "Supervar" },
    { tokLShift        ,  "<<" },
    { tokNe            ,  "!=" },
    { tokText          ,  "\"hello string\"" },
    { tokGe            ,  ">=" },
    { tokNumber        ,  "43" },
    { tokLe            ,  "<=" },
    { tokNumber        ,  "245e4" },
    { tokFunction      ,  "cos(" },
    { tokEq            ,  "==" }
  };

  size_t i;
  memset(buf, 0, sizeof(buf));
  for (i = 0; i < sizeof(tokens)/sizeof(tokens[0]); i++) {
    if (i % 7 == 0)
      strcat(buf, " ");
    strcat(buf, tokens[i].lexem);
  }
  scanner_t *s = scanner_init(buf);

  i = 0;
  while ((t = lexer_nextitem(s))->lexcomp != tokStackEmpty) {
    /*fprintf(stderr, "[%s]: %d\n", t->lexem, t->lexcomp);*/
    assert(strcmp(t->lexem, tokens[i].lexem) == 0);
    assert(t->lexcomp == tokens[i++].lexcomp);
    free(t);
  }
  free(t);
  scanner_destroy(s);
}

void test_unkown() {
  scanner_t *s = scanner_init("..32");
  fprintf(stderr, "EXPECTED ERROR [");
  assert(lexer_nextitem(s) == NULL);
  fprintf(stderr, "] ");
  scanner_destroy(s);
}


int main(int argc, char *argv[]) {
  fprintf(stderr, "Testing... ");
  test_numbers();
  test_lexer();
  test_unkown();
  fprintf(stderr, "done\n");
  return 0;
}

/* vim: set sw=2 sts=2 : */
