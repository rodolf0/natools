#ifndef _LEXER_H_
#define _LEXER_H_

#include <cnash/list.h>
#include "scanner.h"

/* type of tokens: lexical components */
typedef enum _lex_component {
  no_match = -1,
  number = 0,
  op_add,
  op_sub,
  op_mul,
  op_div,
  op_pow,
  paren_open,
  paren_close,
  stack_empty,
  function,
  op_comma,
  op_neg,
  variable,
  op_asig,
  mango_o = 50,
  mango_e,
  mango_c
} lex_component;

/* lexical analysis generated tokens */
typedef struct _token_t {
  lex_component lcomp;
  char *lexem;
  double value;
} token_t;

token_t *token_init(lex_component lc, const char *lexem);
void token_destroy(token_t *t);

/* the lexer interface: returns a list of tokens */
list_t *tokenize(const char *buffer);


#endif /* _LEXER_H_ */

/* vim: set sw=2 sts=2 : */
