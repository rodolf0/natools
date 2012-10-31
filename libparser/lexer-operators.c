#include <stdio.h>

#include "parser/lexer.h"


/* lex math operators */
lexcomp_t tokenize_mathops(scanner_t *s) {
  char op = scanner_advance(s);
  switch (op) {
    case '+':
      return tokPlus;
    case '-':
      return tokMinus;
    case '*':
      return tokTimes;
    case '/':
      return tokDivide;
    case '%':
      return tokModulo;
    case '^':
      return tokPower;
    default:
      scanner_backup(s);
      break;
  }
  return tokNoMatch;
}


/* lex relational operators
 * scan for these before misc so that '='
 * asign op is tokenized correctly */
lexcomp_t tokenize_relops(scanner_t *s) {
  char c = scanner_advance(s);

  if (c == '>') {
    if (scanner_peek(s) == '=') {
      scanner_advance(s);
      return tokGe;
    }
    return tokGt;
  }

  if (c == '<') {
    if (scanner_peek(s) == '=') {
      scanner_advance(s);
      return tokLe;
    }
    return tokLt;
  }

  if (c == '=' && scanner_peek(s) == '=') {
    scanner_advance(s);
    return tokEq;
  }

  if (c == '!' && scanner_peek(s) == '=') {
    scanner_advance(s);
    return tokNe;
  }

  scanner_backup(s);
  return tokNoMatch;
}


/* lex misc operators */
lexcomp_t tokenize_miscops(scanner_t *s) {
  char op = scanner_advance(s);
  switch (op) {
    case '(':
      return tokOParen;
    case ')':
      return tokCParen;
    case ',':
      return tokComma;
    case '=':
      return tokAsign;
    default:
      scanner_backup(s);
      break;
  }
  return tokNoMatch;
}

/* vim: set sw=2 sts=2 : */
