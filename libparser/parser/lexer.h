#ifndef _LEXER_H_
#define _LEXER_H_

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
  /* misc */
  tokStackEmpty, tokNoMatch,
  /* internal use only */
  tokOMango, tokEMango, tokCMango
} lexcomp_t;


/* token definitions */
typedef struct _token_t {
  lexcomp_t lexcomp;
  char *lexem;
} token_t;

token_t * token_init(lexcomp_t lexcomp, char *lexem);
void token_destroy(token_t *t);

typedef struct _scanner_t scanner_t;

/* extract token from the stream (user must later free) */
token_t * lexer_nextitem(scanner_t *s);

typedef struct _lexer_t lexer_t;

lexer_t * lexer_init(scanner_t *s);
void lexer_destroy(lexer_t *l);

token_t * lexer_advance(lexer_t *l);
token_t * lexer_peek(lexer_t *l);
token_t * lexer_current(lexer_t *l);
token_t * lexer_backup(lexer_t *l);

/* drop already scanned tokens without destructing them */
void lexer_shift(lexer_t *l);
/* like shift but free all used tokens */
void lexer_consume(lexer_t *l);

#endif /* _LEXER_H_ */

/* vim: set sw=2 sts=2 : */
