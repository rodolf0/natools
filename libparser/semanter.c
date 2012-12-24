#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser-priv.h"


symbol_t * symbol_number(long double d) {
  symbol_t *s = malloc(sizeof(symbol_t));
  s->type = stNumber;
  s->number = d;
  return s;
}

symbol_t * symbol_variable(char *varname) {
  int len = strlen(varname);
  symbol_t *s = malloc(sizeof(symbol_t) + len + 1);
  s->type = stVariable;
  s->variable = (char*)s + sizeof(symbol_t);
  memcpy(s->variable, varname, len);
  s->variable[len] = '\0';
  return s;
}

symbol_t * symbol_operator(lexcomp_t lc) {
  symbol_t *s = malloc(sizeof(symbol_t));
  if (lc == tokUnaryMinus || lc == tokBitNot || lc == tokNot)
    s->type = stUniOperator;
  else
    s->type = stBinOperator;
  s->operator = lc;
  return s;
}

symbol_t * symbol_function(char *funcname, size_t nargs) {
  int len = strlen(funcname);
  symbol_t *s = malloc(sizeof(symbol_t) + len + 1);
  s->type = stFunction;
  s->func.name = (char*)s + sizeof(symbol_t);
  memcpy(s->func.name, funcname, len);
  s->func.name[len] = '\0';
  s->func.nargs = nargs;
  return s;
}

void symbol_destroy(symbol_t *s) {
  if (!s)
    return;
  free(s);
}

static long double semanter_operator(lexcomp_t lc, long double lhs, long double rhs) {
  switch (lc) {
    /* mathops */
    case tokPlus       : return lhs+rhs;
    case tokMinus      : return lhs-rhs;
    case tokUnaryMinus : return -lhs;
    case tokTimes      : return lhs * rhs;
    case tokDivide     : return lhs / rhs;
    case tokPower      : return pow(lhs, rhs);
    case tokModulo     : return fmod(lhs, rhs);
    /* bitops */
    case tokRShift     : return (int)lhs >> (int)rhs;
    case tokLShift     : return (int)lhs << (int)rhs;
    case tokBitAnd     : return (int)lhs & (int)rhs;
    case tokBitOr      : return (int)lhs | (int)rhs;
    case tokBitXor     : return (int)lhs ^ (int)rhs;
    case tokBitNot     : return ~(int)lhs;
    /* logicops */
    case tokNot        : return !lhs;
    case tokAnd        : return lhs && rhs;
    case tokOr         : return lhs || rhs;
    /* relops */
    case tokEq         : return lhs == rhs;
    case tokNe         : return lhs != rhs;
    case tokGt         : return lhs > rhs;
    case tokLt         : return lhs < rhs;
    case tokGe         : return lhs >= rhs;
    case tokLe         : return lhs <= rhs;

    /* list explicitly so we get compile errors if we miss an operator */
    case tokOParen     : case tokCParen  : case tokComma    :
    case tokNumber     : case tokId      : case tokFunction :
    case tokAsign      : case tokText    :
    case tokTrue       : case tokFalse   :
    case tokStackEmpty : case tokNoMatch :
    case tokOMango     : case tokEMango  : case tokCMango   :
      break;
  }
  return 0;
}


#define unlikely(x) __builtin_expect(!!(x), 0)
int parser_eval(const expr_t *e, long double *r, hashtbl_t *vars) {
  if (!e || !r) {
#ifdef _VERBOSE_
  fprintf(stderr, "eval error: null expression or result var\n");
#endif
    return 1;
  }

  /* load known functions */
  static hashtbl_t *functions = NULL;
  if (unlikely(functions == NULL)) {
    functions = hashtbl_init(NULL, NULL, 0);
    register_functions(functions);
  }
  /* stash constants into whatever symtab we get */
  if (unlikely(vars && !hashtbl_get(vars, "_stashed"))) {
    register_constants(vars);
  }

  const list_t *l = (const list_t*)e;
  const list_node_t *n = l->last;
  list_t *args = list_init(free, NULL);
  const symbol_t *s;

  while (n && (s = (const symbol_t*)n->data)) {
    long double *d = NULL, *v = NULL;
    long double (*f)(list_t*, size_t);

    switch (s->type) {
      case stNumber:
        d = malloc(sizeof(long double));
        *d = s->number;
        list_push(args, d);
        break;

      case stVariable:
        if (!vars) {
#ifdef _VERBOSE_
          fprintf(stderr, "eval error: no symbol table\n");
#endif
          list_destroy(args);
          return 1;
        }
        if (!(v = (long double*)hashtbl_get(vars, s->variable))) {
#ifdef _VERBOSE_
          fprintf(stderr, "eval error: uninitialized variable [%s]\n", s->variable);
#endif
          list_destroy(args);
          return 1;
        }
        d = malloc(sizeof(long double));
        *d = *v;
        list_push(args, d);
        break;

      case stBinOperator:
        /* rhs operand */
        if (!(v = list_pop(args))) {
#ifdef _VERBOSE_
          fprintf(stderr, "eval error: missing rhs operand\n");
#endif
          list_destroy(args);
          return 1;
        }
      case stUniOperator:
        /* lhs operand, don't pop it... use it to store the result too */
        if (!(d = list_peek_head(args))) {
#ifdef _VERBOSE_
          fprintf(stderr, "eval error: missing lhs operand\n");
#endif
          list_destroy(args);
          return 1;
        }
        *d = semanter_operator(s->operator, *d, s->type == stBinOperator ? *v : 0.0);
        free(v);
        break;

      case stFunction:
        if (!(f = hashtbl_get(functions, s->func.name))) {
#ifdef _VERBOSE_
          fprintf(stderr, "eval error: unknown function [%s]\n", s->func.name);
#endif
          list_destroy(args);
          return 1;
        }
        d = malloc(sizeof(long double));
        *d = f(args, s->func.nargs);
        list_push(args, d);
        break;
    }
    n = n->prev;
  }

  if (args->size != 1) {
#ifdef _VERBOSE_
    fprintf(stderr, "eval error: corrupt args stack\n");
#endif
    list_destroy(args);
    return 1;
  }

  long double *d = (long double*)list_pop(args);
  list_destroy(args);
  *r = *d;
  return 0;
}


/* parse symbols out of tokens */
int semanter_reduce(list_t *stack, list_t *partial) {
  size_t funcparams = 0;
  token_t *op;

  while ((op = (token_t*)list_pop(stack))) {
    switch (op->lexcomp) {
      /* binary operators */
      case tokPlus   : case tokMinus  : case tokTimes  :
      case tokDivide : case tokModulo : case tokPower  : case tokUnaryMinus :
      case tokRShift : case tokLShift : case tokBitAnd :
      case tokBitOr  : case tokBitXor : case tokBitNot :
      case tokAnd    : case tokOr     : case tokNot    :
      case tokEq     : case tokNe     : case tokGt     :
      case tokLt     : case tokGe     : case tokLe     :
        list_push(partial, symbol_operator(op->lexcomp));
        break;

      case tokNumber:
        list_push(partial, symbol_number(strtold(op->lexem, NULL)));
        break;
      case tokTrue:
        list_push(partial, symbol_number(1.0));
        break;
      case tokFalse:
        list_push(partial, symbol_number(0.0));
        break;
      case tokId:
        list_push(partial, symbol_variable(op->lexem));
        break;
      case tokFunction:
        list_push(partial, symbol_function(op->lexem, funcparams));
        break;

      /* ignore these, no semantic value */
      case tokEMango  :
        funcparams++;
      case tokComma   : case tokStackEmpty : case tokNoMatch :
      case tokCMango  : case tokOParen     : case tokCParen  :
      default:
        break;

      /* reduction done */
      case tokOMango:
        token_destroy(op);
        return 0;
    }
    token_destroy(op);
  }
  return 0;
}

/* vim: set sw=2 sts=2 : */
