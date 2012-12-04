#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser-priv.h"

symbol_t * symbol_number(double d) {
  symbol_t *s = malloc(sizeof(symbol_t));
  s->t = stNumber;
  s->dVal = d;
  return s;
}

symbol_t * symbol_variable(char *varname) {
  int len = strlen(varname);
  symbol_t *s = malloc(sizeof(symbol_t) + len + 1);
  s->t = stVariable;
  s->sVal = (char*)s + sizeof(symbol_t);
  memcpy(s->sVal, varname, len);
  s->sVal[len] = '\0';
  return s;
}

void symbol_destroy(symbol_t *s) {
  if (!s)
    return;
  if (s->t == stVariable)
    free(s->sVal);
  free(s);
}

double lookup_symbol(hashtbl_t *symtab, symbol_t *s) {
  if (!symtab || !s) {
    fprintf(stderr, "Invalid symbol or symbol table\n");
    return 0.0;
  }
  if (s->t == stNumber)
    return s->dVal;
  else if (s->t == stVariable) {
    double *d;
    if ((d = (double*)hashtbl_get(symtab, s->sVal))) {
      fprintf(stderr, "semantic error: uninitialized variable [%s]\n", s->sVal);
      return 0.0;
    }
    return *d;
  }
  fprintf(stderr, "semantic error: unkown type %d\n", s->t);
  return 0.0;
}

static double semanter_mathops(lexcomp_t lc, double lhs, double rhs) {
  switch (lc) {
    case tokPlus       : return lhs+rhs;
    case tokMinus      : return lhs-rhs;
    case tokUnaryMinus : return -rhs;
    case tokTimes      : return lhs * rhs;
    case tokDivide     : return lhs / rhs;
    case tokModulo     : return pow(lhs, rhs);
    case tokPower      : return fmod(lhs, rhs);
    default:
      return 0.0;
  }
}

static int semanter_bitops(lexcomp_t lc, int lhs, int rhs) {
  switch (lc) {
    case tokRShift: return lhs >> rhs;
    case tokLShift: return lhs << rhs;
    case tokBitAnd: return lhs & rhs;
    case tokBitOr:  return lhs | rhs;
    case tokBitXor: return lhs ^ rhs;
    case tokBitNot: return ~rhs;
    default:
      return 0;
  }
}

static int semanter_logicops(lexcomp_t lc, int lhs, int rhs) {
  switch (lc) {
    case tokNot: return !rhs;
    case tokAnd: return lhs && rhs;
    case tokOr:  return lhs || rhs;
    default:
      return 0;
  }
}

static int semanter_relops(lexcomp_t lc, double lhs, double rhs) {
  switch (lc) {
    case tokEq: return lhs == rhs;
    case tokNe: return lhs != rhs;
    case tokGt: return lhs > rhs;
    case tokLt: return lhs < rhs;
    case tokGe: return lhs >= rhs;
    case tokLe: return lhs <= rhs;
    default:
      return 0;
  }
}


/* do semantic evaluation of the parser stack on reduction */
int semantic_eval_1(parser_t *p) {
  size_t funcparams = 0;
  token_t *op;

  while ((op = (token_t*)list_pop(p->stack))->lexcomp != tokOMango) {
    double lhs = 0.0, rhs = 0.0, *dp = NULL, d;
    symbol_t *s = NULL;

    switch (op->lexcomp) {
      /* ignore these, no semantic value */
      case tokOParen:
      case tokCParen:
      case tokComma:
      case tokStackEmpty:
      case tokNoMatch:
        break;

      /* mathops */
      case tokPlus:
      case tokMinus:
      case tokTimes:
      case tokDivide:
      case tokModulo:
      case tokPower:
        s = (symbol_t*)list_pop(p->partial);
        rhs = lookup_symbol(p->symbol_table, s);
        symbol_destroy(s);
      case tokUnaryMinus:
        s = (symbol_t*)list_pop(p->partial);
        lhs = lookup_symbol(p->symbol_table, s);
        symbol_destroy(s);
        d = semanter_mathops(op->lexcomp, lhs, rhs);
        list_push(p->partial, symbol_number(d));
        break;

      /* bitops */
      case tokRShift:
      case tokLShift:
      case tokBitAnd:
      case tokBitOr:
      case tokBitXor:
        s = (symbol_t*)list_pop(p->partial);
        rhs = lookup_symbol(p->symbol_table, s);
        symbol_destroy(s);
      case tokBitNot:
        s = (symbol_t*)list_pop(p->partial);
        lhs = lookup_symbol(p->symbol_table, s);
        symbol_destroy(s);
        d = (double)semanter_bitops(op->lexcomp, (int)lhs, (int)rhs);
        list_push(p->partial, symbol_number(d));
        list_push(p->partial, symbol_number(d));
        break;

      /* logicops */
      case tokAnd:
      case tokOr:
        s = (symbol_t*)list_pop(p->partial);
        rhs = lookup_symbol(p->symbol_table, s);
        symbol_destroy(s);
      case tokNot:
        s = (symbol_t*)list_pop(p->partial);
        lhs = lookup_symbol(p->symbol_table, s);
        symbol_destroy(s);
        d = (double)semanter_logicops(op->lexcomp, lhs, rhs);
        list_push(p->partial, symbol_number(d));
        break;

      /* relops */
      case tokEq:
      case tokNe:
      case tokGt:
      case tokLt:
      case tokGe:
      case tokLe:
        s = (symbol_t*)list_pop(p->partial);
        rhs = lookup_symbol(p->symbol_table, s);
        symbol_destroy(s);
        s = (symbol_t*)list_pop(p->partial);
        lhs = lookup_symbol(p->symbol_table, s);
        symbol_destroy(s);
        d = (double)semanter_relops(op->lexcomp, lhs, rhs);
        list_push(p->partial, symbol_number(d));
        break;

      /* no partials involved, just the lexem being used */
      case tokNumber:
        list_push(p->partial, symbol_number(atof(op->lexem)));
        break;
      case tokTrue:
        list_push(p->partial, symbol_number(1.0));
        break;
      case tokFalse:
        list_push(p->partial, symbol_number(0.0));
        break;
      case tokId:
        list_push(p->partial, symbol_variable(op->lexem));
        break;

      case tokAsign:
        s = (symbol_t*)list_pop(p->partial);
        rhs = lookup_symbol(p->symbol_table, s);
        symbol_destroy(s);
        s = (symbol_t*)list_pop(p->partial);
        if ((dp = (double*)hashtbl_get(p->symbol_table, s->sVal))) {
          *dp = rhs;
        } else {
          dp = malloc(sizeof(double));
          *dp = rhs;
          hashtbl_insert(p->symbol_table, s->sVal, dp);
        }
        symbol_destroy(s);
        break;

      case tokFunction:
      case tokText:
        break;

      case tokEMango:
        funcparams++;
        break;

      case tokCMango:
      case tokOMango:
        break;
    }
    token_destroy(op);
  }
  /* destroy opening mango */
  token_destroy(op);
  return 0;
}

/* vim: set sw=2 sts=2 : */
