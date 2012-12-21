#ifndef _PARSER_PRIV_H_
#define _PARSER_PRIV_H_

#include "baas/list.h"
#include "baas/hashtbl.h"
#include "parser/lexer.h"
#include "parser/parser.h"

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
typedef op_prec_t (*precedencefn)(lexcomp_t op1, lexcomp_t op2);
op_prec_t parser_precedence_1(lexcomp_t op1, lexcomp_t op2);

/* token adjustor for corrections considering lookahead */
typedef token_t * (*adjustfn)(token_t *t, token_t *prev);
token_t * adjust_token_1(token_t *t, token_t *prev);

/* semantic evaluation of the parser's output */
typedef int (*semanticfn)(parser_t *p);
int semantic_eval_1(parser_t *p);
int semantic_evaluation(parser_t *p);

typedef struct _parser_t {
  precedencefn p;
  adjustfn adjust;
  semanticfn reduce;
  hashtbl_t *symbol_table;
  hashtbl_t *function_table;
  list_t *stack;
  list_t *partial;
} parser_t;


typedef enum {
  stNumber,
  stVariable,
  stBinOperator,
  stUniOperator,
  stFunction,
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
int pop_operand(parser_t *p, long double *r);

/* prototype of known functions */
typedef int (*function_t)(parser_t *p, size_t nargs, long double *r);
int register_functions(parser_t *p);
int register_constants(parser_t *p);

#endif /* _PARSER_H_PARSER_H_ */

/* vim: set sw=2 sts=2 : */
