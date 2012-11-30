#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "parser/parser.h"

int semanter_eval(parser_t *p) {
  static int (*semanters[])(parser_t*) = {
    semanter_mathops,
    semanter_bitops,
  };

  token *op;
  size_t i;
  size_t funcparams = 0;

  while ((op = (token_t*)list_pop(p->stack))->lexcomp != tokOMango) {

    if (op->lexcomp == tokEMango)
      funcparams++;

    for (i = 0; i < sizeof(semanters)/sizeof(semanters[0]); i++) {
      if (semanters[i](p) == 0) {
        break;
      }
    }

    token_destroy(op);
  }
  /* opening mango */
  token_destroy(op);
}

double * resolve_symbol(symbol_t *s, hashtbl_t *symtab) {
  if (!s)
    return NULL;
  else if (s && s->t == stNumber)
    return &s->dVal;
  else if (s && s->t == stVariable && symtab)
    return (double*)hashtbl_get(symtab, s->sVal);
  return NULL;
}

int semanter_mathops(parser_t *p) {
  token_t *op = (token_t*)list_pop(p->stack);
  symbol_t *rhs = (symbol_t*)list_pop(p->partial);
  symbol_t *lhs = (symbol_t*)list_pop(p->partial);
  double *rhv = resolve_symbol(rhs, p->symbol_table),
         *lhv = resolve_symbol(lhs, p->symbol_table);
  double r;

  switch (op->lexcomp) {
    case tokPlus:
      r = *lhv + *rhv;
      break;
    case tokMinus:
      r = *lhv - *rhv;
      break;
    case tokUnaryMinus:
      if (lhs) list_push(p->partial, lhs);
      r = -(*rhv);
      break;
    case tokTimes:
      r = (*lhv) * (*rhv);
      break;
    case tokDivide:
      r = *lhv / *rhv;
      break;
    case tokModulo:
      r = fmod(*lhv, *rhv);
      break;
    case tokPower:
      r = pow(*lhv, *rhv);
      break;
    default:
      /* return the pop'ed */
      if (lhs) list_push(p->partial, lhs);
      if (rhs) list_push(p->partial, rhs);
      list_push(p->stack, op);
      return 1;
  }
  if (lhs) symbol_destroy(lhs);
  if (rhs) symbol_destroy(rhs);
  token_destroy(op);
  list_push(p->partial, symbol_number(r));
  return 0;
}


int semanter_bitops(parser_t *p) {
  token_t *op = (token_t*)list_pop(p->stack);
  symbol_t *rhs = (symbol_t*)list_pop(p->partial);
  symbol_t *lhs = (symbol_t*)list_pop(p->partial);
  double *rhv = resolve_symbol(rhs, p->symbol_table),
         *lhv = resolve_symbol(lhs, p->symbol_table);
  double r;
  switch (op->lexcomp) {
    case tokRShift:
      r = (int)*lhv >> (int)*rhv;
      break;
    case tokLShift:
      r = (int)*lhv << (int)*rhv; 
      break;
    case tokLBitAnd:
      r = (int)*lhv & (int)*rhv;
      break;
    case tokLBitOr:
      r = (int)*lhv | (int)*rhv;
      break;
    case tokLBitXor: 
      r = (int)*lhv ^ (int)*rhv; 
      break;
    case tokLBitNot: 
      if (lhs) list_push(p->partial, lhs);
      r = ~(int)*rhv; 
      break;
    default:
      /* return the pop'ed */
      if (lhs) list_push(p->partial, lhs);
      if (rhs) list_push(p->partial, rhs);
      list_push(p->stack, op);
      return 1;
  }
  if (lhs) symbol_destroy(lhs);
  if (rhs) symbol_destroy(rhs);
  token_destroy(op);
  list_push(p->partial, symbol_number(r));
  return 0;
}


int semanter_logicops(parser_t *p) {
  token_t *op = (token_t*)list_pop(p->stack);
  symbol_t *rhs = (symbol_t*)list_pop(p->partial);
  symbol_t *lhs = (symbol_t*)list_pop(p->partial);
  double *rhv = resolve_symbol(rhs, p->symbol_table),
         *lhv = resolve_symbol(lhs, p->symbol_table);
  double r;
  switch (op->lexcomp) {
    case tokNot:
      if (lhs) list_push(p->partial, lhs);
      r = !(*rhv);
      break;
    case tokAnd:
      r = *lhv && *rhv; 
      break;
    case tokOr:
      r = *lhv || *rhv; 
      break;
    default:
      /* return the pop'ed */
      if (lhs) list_push(p->partial, lhs);
      if (rhs) list_push(p->partial, rhs);
      list_push(p->stack, op);
      return 1;
  }
  if (lhs) symbol_destroy(lhs);
  if (rhs) symbol_destroy(rhs);
  list_push(p->partial, symbol_number(r));
  return 0;
}


int semanter_relops(parser_t *p) {
  token_t *op = (token_t*)list_pop(p->stack);
  symbol_t *rhs = (symbol_t*)list_pop(p->partial);
  symbol_t *lhs = (symbol_t*)list_pop(p->partial);
  double *rhv = resolve_symbol(rhs, p->symbol_table),
         *lhv = resolve_symbol(lhs, p->symbol_table);
  double r;
  switch (op->lexcomp) {
    case tokEq:
      r = *lhv == *rhv;
      break;
    case tokNe:
      r = *lhv != *rhv; 
      break;
    case tokGt:
      r = *lhv > *rhv;
      break;
    case tokLt:
      r = *lhv < *rhv;
      break;
    case tokGe: 
      r = *lhv >= *rhv; 
      break;
    case tokLe: 
      r = *lhv <= *rhv; 
      break;
    default:
      /* return the pop'ed */
      if (lhs) list_push(p->partial, lhs);
      if (rhs) list_push(p->partial, rhs);
      list_push(p->stack, op);
      return 1;
  }
  if (lhs) symbol_destroy(lhs);
  if (rhs) symbol_destroy(rhs);
  list_push(p->partial, symbol_number(r));
  return 0;
}


int semanter_miscops(parser_t *p) {
  token_t *op = (token_t*)list_pop(p->stack);
  symbol_t *lhs, *rhs;

  switch (op->lexcomp) {
    case tokOparen:
    case tokCparen:
    case tokComma:
      /* ignore these, no semantic value */
      break;

    case tokAsign:
      /* parser table already checked rhs is an id */
      *rhs = (symbol_t*)list_pop(p->partial);
      *lhs = (symbol_t*)list_pop(p->partial);
      double rhv = *resolve_symbol(rhs, p->symbol_table);
      /* TODO: use hashtbl_set (not yet written) */
      hashtbl_remove(p->symbol_table, lhs->sVal);
      hashtbl_insert(p->symbol_table, lhs->sVal, symbol_number());
      break;

    default:
      /* return the pop'ed */
      list_push(p->stack, op);
      return 1;
  }
  return 0;
}


int semanter_identifiers(parser_t *p) {
  token_t *op = (token_t*)list_pop(p->stack);
  double id = 0.0;

  switch (op->lexcomp) {
    case tokNumber:
      id = atof(op->lexem);
      break;
    case tokId:
      id = 
      break;
    case tokFunction:
      if (double *var = (double*)hashtbl_get(p->symbol_table, op->lexem)) {
        ***
      } else {
        fprintf(stderr, "semantic error: undefined function %s\n", op->lexem);
        return 2;
      }
      break;
    case tokTrue:
      id = 1.0;
      break;
    case tokFalse:
      id = 0.0;
      break;
    default:
      /* return the pop'ed */
      if (lhs) list_push(p->partial, lhs);
      if (rhs) list_push(p->partial, rhs);
      list_push(p->stack, op);
      return 1;
  }
  list_push(p->partial, symbol_init(op, r));
  if (lhs) symbol_destroy(lhs);
  if (rhs) symbol_destroy(rhs);
  return 0;
}

/* vim: set sw=2 sts=2 : */
