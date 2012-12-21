#ifndef _PARSER_PRIV_H_
#define _PARSER_PRIV_H_

#include "baas/list.h"
#include "parser/lexer.h"

typedef struct _expr_t expr_t;

/* precedence relation between two operators */
typedef enum {
  LT = -1,
  EQ = 0,
  GT = 1,
  E0, /* empty buffer and stack, end of parsing */
  E1, /* non-variable assignment */
  E2, /* wrong argument type for operator */
  E3, /* expected binary operator or eol */
  E4, /* unbalanced open parenthesis */
  E5, /* comma only allowed between function arguments */
  E6, /* unbalanced closing parenthesis */
  E7  /* internal error */
} op_prec_t;

/* return the precedence relation of two operators */
op_prec_t parser_precedence(lexcomp_t op1, lexcomp_t op2);
/* adjust token type based on previous one (eg: unary operators) */
token_t * adjust_token(token_t *t, token_t *prev);
/* semantic evaluation of the parser's output */
int semanter_reduce(list_t *stack, list_t *partial);

/* parsed symbols */
typedef enum {
  stNumber,
  stVariable,
  stBinOperator,
  stUniOperator,
  stFunction,
  /* stAsignment, */
} symtype_t;

typedef struct _symbol_t {
  symtype_t type;
  union {
    long double number;
    char *variable;
    lexcomp_t operator;
    struct {
      char *name;
      size_t nargs;
    } func;
  };
} symbol_t;

symbol_t * symbol_number(long double d);
symbol_t * symbol_variable(char *varname);
symbol_t * symbol_operator(lexcomp_t lc);
symbol_t * symbol_function(char *funcname, size_t nargs);
void symbol_destroy(symbol_t *s);

#endif /* _PARSER_H_PARSER_H_ */

/* vim: set sw=2 sts=2 : */
