#include <string.h>
#include <stdio.h>
#include "parser/lexer.h"

/* tokenizer FSM functions */
typedef void (*statefn)(void);
typedef statefn (*nextstate)(scanner_t *s);


/* check if the current string is a reserved word */
static lexcomp_t reserved_word(scanner_t *s) {
  /* TODO: don't get into the scanner, use it's API */
  if (!strncmp(s->buffer + s->start, "true", s->length < 4 ? 4 : s->length))
    return tokTrue;
  if (!strncmp(s->buffer + s->start, "false", s->length < 5 ? 5 : s->length))
    return tokFalse;
  if (!strncmp(s->buffer + s->start, "and", s->length < 3 ? 3 : s->length))
    return tokAnd;
  if (!strncmp(s->buffer + s->start, "not", s->length < 3 ? 3 : s->length))
    return tokNot;
  if (!strncmp(s->buffer + s->start, "or", s->length < 2 ? 2 : s->length))
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
    fprintf(stderr, "lexer error: expected fractional part\n");
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
    fprintf(stderr, "lexer error: expected exponent\n");
    return error;
  }

  while (is_num(scanner_advance(s)));

  scanner_backup(s);
  return done;
}
/* vim: set sw=2 sts=2 : */
