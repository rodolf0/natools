#ifndef _LEXER_H_
#define _LEXER_H_

#include "scanner.h"

/* lexical components */
typedef enum _lexcomp_t {
  /* math operators */
  tokPlus, tokMinus, tokUnaryMinus,
  tokTimes, tokDivide, tokModulo, tokPower,
  /* bitwise operators */
  tokRShift, tokLShift,
  tokBitAnd, tokBitOr, tokBitXor, tokBitNot,
  /* logical operators */
  tokNot, tokAnd, tokOr,
  /* logical constants */
  tokTrue, tokFalse,
  /* relational operators */
  tokEq, tokNe, tokGt, tokLt, tokGe, tokLe,
  /* misc operators */
  tokOParen, tokCParen, tokComma, tokAsign,
  /* numbers, text, ids... */
  tokNumber, tokText, tokId, tokFunction,
  /* tokNoMatch being last indicates number of known lc's */
  tokStackEmpty, tokNoMatch
} lexcomp_t;


/* token definitions */
typedef struct _token_t {
  lexcomp_t lexcomp;
  char *lexem;
} token_t;


token_t * token_init(lexcomp_t lexcomp, const char *lexem);
void token_destroy(token_t *t);

/* extract token from the stream (user must later free) */
token_t * lexer_nextitem(scanner_t *s);

#endif /* _LEXER_H_ */

/* vim: set sw=2 sts=2 : */
