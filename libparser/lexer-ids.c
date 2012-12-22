#include <string.h>
#ifdef _VERBOSE_
#include <stdio.h>
#endif
#include "parser/lexer.h"


int is_num(char x);
int is_alpha(char x);

/* tokenizer FSM functions */
typedef void (*statefn)(void);
typedef statefn (*nextstate)(scanner_t *s);


/* interpret a number token of the form:
 * n+(.n+)?((e|E)(+|-)?n+)? */
static statefn integer(scanner_t *s);
static statefn fractional(scanner_t *s);
static statefn exponent(scanner_t *s);
static void done() { }
static void error() { }

lexcomp_t tokenize_number(scanner_t *s) {
  if (!is_num(scanner_peek(s)))
    return tokNoMatch;

  nextstate state = integer;
  while ((statefn)state != done &&
         (statefn)state != error)
    state = (nextstate)(*state)(s);

  return ((statefn)state == error ? tokNoMatch : tokNumber);
}

/* identify the integer part of a number */
static statefn integer(scanner_t *s) {
  /* consume all numbers */
  while (is_num(scanner_advance(s)));

  if (scanner_current(s) == '.')
    return (statefn)fractional;

  if (scanner_current(s) == 'e' ||
      scanner_current(s) == 'E')
    return (statefn)exponent;

  /* this is no longer part of a number, backup */
  scanner_backup(s);
  return done;
}

/* identify the fractional part of a number */
static statefn fractional(scanner_t *s) {
  if (!is_num(scanner_peek(s))) {
#ifdef _VERBOSE_
    fprintf(stderr, "lexer error: expected fractional part\n");
#endif
    return error;
  }

  while (is_num(scanner_advance(s)));

  if (scanner_current(s) == 'e' ||
      scanner_current(s) == 'E')
    return (statefn)exponent;

  scanner_backup(s);
  return done;
}

/* identify the exponent of a number */
static statefn exponent(scanner_t *s) {
  /* accept exponent signs if any */
  char c = scanner_peek(s);
  if (c == '+' || c == '-')
    scanner_advance(s);

  if (!is_num(scanner_peek(s))) {
#ifdef _VERBOSE_
    fprintf(stderr, "lexer error: expected exponent\n");
#endif
    return error;
  }

  while (is_num(scanner_advance(s)));

  scanner_backup(s);
  return done;
}


/* check if the current string is a reserved word */
static int cmp_true(char *start, size_t len) {
  return strncmp(start, "true", len < 4 ? 4 : len);
}
static int cmp_false(char *start, size_t len) {
  return strncmp(start, "false", len < 5 ? 5 : len);
}
static int cmp_and(char *start, size_t len) {
  return strncmp(start, "and", len < 3 ? 3 : len);
}
static int cmp_not(char *start, size_t len) {
  return strncmp(start, "not", len < 3 ? 3 : len);
}
static int cmp_or(char *start, size_t len) {
  return strncmp(start, "or", len < 2 ? 2 : len);
}

static lexcomp_t reserved_word(scanner_t *s) {
  if (scanner_apply(s, (acceptfn)cmp_true) == 0)
    return tokTrue;
  if (scanner_apply(s, (acceptfn)cmp_false) == 0)
    return tokFalse;
  if (scanner_apply(s, (acceptfn)cmp_and) == 0)
    return tokAnd;
  if (scanner_apply(s, (acceptfn)cmp_not) == 0)
    return tokNot;
  if (scanner_apply(s, (acceptfn)cmp_or) == 0)
    return tokOr;
  return tokNoMatch;
}


/* lex variable and function names */
lexcomp_t tokenize_identifier(scanner_t *s) {
  if (!is_alpha(scanner_peek(s)))
    return tokNoMatch;

  char c = scanner_advance(s);
  while (is_alpha(c) || is_num(c))
    c = scanner_advance(s);
  scanner_backup(s);

  lexcomp_t rw = reserved_word(s);
  if (rw != tokNoMatch)
    return rw;

  if (scanner_advance(s) == '(')
    return tokFunction;

  scanner_backup(s);
  return tokId;
}


/* lex text */
lexcomp_t tokenize_text(scanner_t *s) {
  if (scanner_peek(s) != '"')
    return tokNoMatch;

  scanner_advance(s);
  while (scanner_advance(s) != '"');

  return tokText;
}

/* vim: set sw=2 sts=2 : */
