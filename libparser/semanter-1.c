#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser-priv.h"

symbol_t * symbol_number(long double d) {
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
  free(s);
}


int pop_operand(parser_t *p, long double *r) {
  if (p->partial->size < 1) {
#ifdef _VERBOSE_
    fprintf(stderr, "syntactic error: missing operand\n");
#endif
    return 1;
  }
  symbol_t *s = (symbol_t*)list_pop(p->partial);
  if (!s) {
#ifdef _VERBOSE_
    fprintf(stderr, "Invalid symbol\n");
#endif
    return 1;
  }

  int error = 0;
  switch (s->t) {
    case stNumber:
      *r = s->dVal;
      break;

    case stVariable:
      if (!p->symbol_table) {
#ifdef _VERBOSE_
        fprintf(stderr, "Invalid symbol table\n");
        error = 1;
#endif
      }
      long double *d;
      if (!(d = (long double*)hashtbl_get(p->symbol_table, s->sVal))) {
#ifdef _VERBOSE_
        fprintf(stderr, "semantic error: uninitialized variable [%s]\n", s->sVal);
#endif
        error = 2;
      } else
        *r = *d;
      break;

    default:
#ifdef _VERBOSE_
  fprintf(stderr, "semantic error: unkown type %d\n", s->t);
#endif
      break;
  }
  symbol_destroy(s);
  return error;
}


static long double semanter_mathops(lexcomp_t lc, long double lhs, long double rhs) {
  switch (lc) {
    case tokPlus       : return lhs+rhs;
    case tokMinus      : return lhs-rhs;
    case tokUnaryMinus : return -lhs;
    case tokTimes      : return lhs * rhs;
    case tokDivide     : return lhs / rhs;
    case tokPower      : return pow(lhs, rhs);
    case tokModulo     : return fmod(lhs, rhs);
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
    case tokBitNot: return ~lhs;
    default:
      return 0;
  }
}

static int semanter_logicops(lexcomp_t lc, int lhs, int rhs) {
  switch (lc) {
    case tokNot: return !lhs;
    case tokAnd: return lhs && rhs;
    case tokOr:  return lhs || rhs;
    default:
      return 0;
  }
}

static int semanter_relops(lexcomp_t lc, long double lhs, long double rhs) {
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
  int error = 0;

  while (error == 0) {
    op = (token_t*)list_pop(p->stack);
    long double lhs = 0.0, rhs = 0.0, d;
    symbol_t *s = NULL;
    function_t fp;

    switch (op->lexcomp) {
      /* mathops */
      case tokPlus:
      case tokMinus:
      case tokTimes:
      case tokDivide:
      case tokModulo:
      case tokPower:
        if ((error = pop_operand(p, &rhs))) break;
      case tokUnaryMinus:
        if ((error = pop_operand(p, &lhs))) break;
        d = semanter_mathops(op->lexcomp, lhs, rhs);
        list_push(p->partial, symbol_number(d));
        break;

      /* bitops */
      case tokRShift:
      case tokLShift:
      case tokBitAnd:
      case tokBitOr:
      case tokBitXor:
        if ((error = pop_operand(p, &rhs))) break;
      case tokBitNot:
        if ((error = pop_operand(p, &lhs))) break;
        d = (long double)semanter_bitops(op->lexcomp, (int)lhs, (int)rhs);
        list_push(p->partial, symbol_number(d));
        break;

      /* logicops */
      case tokAnd:
      case tokOr:
        if ((error = pop_operand(p, &rhs))) break;
      case tokNot:
        if ((error = pop_operand(p, &lhs))) break;
        d = (long double)semanter_logicops(op->lexcomp, lhs, rhs);
        list_push(p->partial, symbol_number(d));
        break;

      /* relops */
      case tokEq:
      case tokNe:
      case tokGt:
      case tokLt:
      case tokGe:
      case tokLe:
        if ((error = pop_operand(p, &rhs))) break;
        if ((error = pop_operand(p, &lhs))) break;
        d = (long double)semanter_relops(op->lexcomp, lhs, rhs);
        list_push(p->partial, symbol_number(d));
        break;

      /* no partials involved, just the lexem being used */
      case tokNumber:
        list_push(p->partial, symbol_number(strtold(op->lexem, NULL)));
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
        if ((error = pop_operand(p, &rhs))) break;
        if (p->partial->size < 1) {
#ifdef _VERBOSE_
          fprintf(stderr, "syntactic error: missing variable name\n");
#endif
          error = 1;
          break;
        }
        s = (symbol_t*)list_pop(p->partial);
        parser_setvar(p, s->sVal, rhs);
        symbol_destroy(s);
        list_push(p->partial, symbol_number(rhs));
        break;

      case tokFunction:
        if (!(fp = (function_t)hashtbl_get(p->function_table, op->lexem))) {
#ifdef _VERBOSE_
          fprintf(stderr, "semantic error: unknown function %s\n", op->lexem);
#endif
          error = 2;
          break;
        }
        if ((error = fp(p, funcparams, &rhs))) {
#ifdef _VERBOSE_
          fprintf(stderr, "function error: error un function call %s\n", op->lexem);
#endif
          break;
        }
        list_push(p->partial, symbol_number(rhs));
        break;

      case tokEMango:
        funcparams++;
        break;

      /* ignore these, no semantic value */
      default:
      case tokCMango:
      case tokOParen:
      case tokCParen:
      case tokComma:
      case tokStackEmpty:
      case tokNoMatch:
        break;

      /* reduction done */
      case tokOMango:
        error = -1;
        break;
    }
    token_destroy(op);
    if (error > 0)
      return error;
  }
  return 0;
}

/* vim: set sw=2 sts=2 : */
