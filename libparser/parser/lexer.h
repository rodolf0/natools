#ifndef _LEXER_H_
#define _LEXER_H_

#include "scanner.h"

/* lexical components */
typedef enum _lexcomp_t {
  /* math operators */
  tokPlus = 1,
  tokMinus,
  tokTimes,
  tokDivide,
  tokModulo,
  tokPower,
  /* misc operators */
  tokOParen,
  tokCParen,
  tokComma,
  tokAsign,
  /* logical operators */
  tokNot,
  tokAnd,
  tokOr,
  tokTrue,
  tokFalse,
  /* relational operators */
  tokEq,
  tokNe,
  tokGt,
  tokLt,
  tokGe,
  tokLe,
  /* numbers, text, ids... */
  tokNumber,
  tokText,
  tokId,
  tokFunction,
  /* parser specific */
  tokOMango = 100,
  tokEMango,
  tokCMango,
  tokStackEmpty,
  tokNoMatch
} lexcomp_t;

/* known tokenizers */
lexcomp_t tokenize_identifier(scanner_t *s);
lexcomp_t tokenize_text(scanner_t *s);
lexcomp_t tokenize_number(scanner_t *s);
lexcomp_t tokenize_mathops(scanner_t *s);
lexcomp_t tokenize_relops(scanner_t *s);
lexcomp_t tokenize_miscops(scanner_t *s);


/* utility functions */
int is_white(char x);
int is_alpha(char x);
int is_num(char x);


/* token definitions */
typedef struct _token_t {
  lexcomp_t lexcomp;
  char *lexem;
} token_t;

/* extract token from the stream (user must later free) */
token_t * lexer_nextitem(scanner_t *s);

#endif /* _LEXER_H_ */

/* vim: set sw=2 sts=2 : */
