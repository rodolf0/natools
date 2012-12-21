/* http://en.wikipedia.org/wiki/Operator-precedence_grammar */

#include <stdio.h>
#include <stdlib.h>
#include "parser-priv.h"

parser_t * parser_create() {
  parser_t *p = (parser_t*)malloc(sizeof(parser_t));
  p->p = parser_precedence_1;
  p->adjust = adjust_token_1;
  p->reduce = semantic_evaluation;
  p->symbol_table = hashtbl_init(free, NULL);
  p->function_table = hashtbl_init(NULL, NULL);
  p->stack = NULL;
  p->partial = NULL;
  /* preload symbol table */
  /*if (register_constants(p)) {*/
    /*parser_destroy(p);*/
    /*return NULL;*/
  /*}*/
  /*if (register_functions(p)) {*/
    /*parser_destroy(p);*/
    /*return NULL;*/
  /*}*/
  return p;
}

void parser_destroy(parser_t *p) {
  if (!p)
    return;
  hashtbl_destroy(p->symbol_table);
  hashtbl_destroy(p->function_table);
  free(p);
}


int parser_setvar(parser_t *p, char *var, long double val) {
  if (!p || !var)
    return 1;
  long double *v;
  if ((v = (long double*)hashtbl_get(p->symbol_table, var))) {
    *v = val;
  } else {
    v = malloc(sizeof(long double));
    *v= val;
    hashtbl_insert(p->symbol_table, var, v);
  }
  return 0;
}

expr_t * parser_compile(parser_t *p, scanner_t *s) {
  if (!p || !s) {
#ifdef _VERBOSE_
    fprintf(stderr, "Invalid parser or scanner\n");
#endif
    return NULL;
  }

  p->stack = list_init((free_func_t)token_destroy, NULL);
  list_push(p->stack, token_init(tokStackEmpty, ""));
  p->partial = list_init(free, NULL);
  token_t *st, *bf = p->adjust(lexer_nextitem(s), NULL);

  int error = 0;
  while (error == 0) {
    st = (token_t*)list_peek_head(p->stack);
    switch (p->p(st->lexcomp, bf->lexcomp)) {
      case LT:
        list_push(p->stack, token_init(tokOMango, ""));
        list_push(p->stack, bf);
        bf = p->adjust(lexer_nextitem(s), bf);
        break;
      case EQ:
        list_push(p->stack, token_init(tokEMango, ""));
        list_push(p->stack, bf);
        bf = p->adjust(lexer_nextitem(s), bf);
        break;
      case GT:
        error = p->reduce(p);
        break;

      case E0:
        error = -1; break; /* parsing finished */
      case E1:
#ifdef _VERBOSE_
        fprintf(stderr, "syntactic error: non-variable assignment\n");
#endif
        error = 1; break;
      case E2:
#ifdef _VERBOSE_
        fprintf(stderr, "syntactic error: wrong argument type for operator\n");
#endif
        error = 2; break;
      case E3:
#ifdef _VERBOSE_
        fprintf(stderr, "syntactic error: expected binary operator or eol\n");
#endif
        error = 3; break;
      case E4:
#ifdef _VERBOSE_
        fprintf(stderr, "syntactic error: unbalanced open parenthesis\n");
#endif
        error = 4; break;
      case E5:
#ifdef _VERBOSE_
        fprintf(stderr, "syntactic error: comma only allowed bt function arguments\n");
#endif
        error = 5; break;
      case E6:
#ifdef _VERBOSE_
        fprintf(stderr, "syntactic error: unbalanced closing parenthesis\n");
#endif
        error = 6; break;
      default:
      case E7:
#ifdef _VERBOSE_
        fprintf(stderr, "syntactic error: stack [%d:%s], buffer [%d:%s]\n",
                st->lexcomp, st->lexem, bf->lexcomp, bf->lexem);
#endif
        error = 7; break;
    }
  }

  token_destroy(bf);
  list_destroy(p->stack);
  p->stack = NULL;

  list_t *expr;
  if (error <= 0) {
    expr = p->partial;
    p->partial = NULL;
  } else {
    expr = NULL;
    list_destroy(p->partial);
  }
  return (expr_t*)expr;
}

/* vim: set sw=2 sts=2 : */
