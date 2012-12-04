/* http://en.wikipedia.org/wiki/Operator-precedence_grammar */

#include <stdio.h>
#include <stdlib.h>
#include "parser-priv.h"

parser_t * parser_create() {
  parser_t *p = (parser_t*)malloc(sizeof(parser_t));
  p->p = parser_precedence_1;
  p->adjust = adjust_token_1;
  p->reduce = semantic_eval_1;
  p->symbol_table = hashtbl_init(free, NULL);
  p->stack = NULL;
  p->partial = NULL;
  return p;
}

void parser_destroy(parser_t *p) {
  if (!p)
    return;
  hashtbl_destroy(p->symbol_table);
  free(p);
}

double parser_eval(parser_t *p, scanner_t *s) {
  if (!p || !s) {
    fprintf(stderr, "Invalid parser or scanner\n");
    return 0.0;
  }

  p->stack = list_init((free_func_t)token_destroy, NULL);
  p->partial = list_init(free, NULL);
  token_t *st, *bf = p->adjust(lexer_nextitem(s), NULL);

  int done = 0;
  while (!done) {
    st = (token_t*)list_peek_head(p->stack);
    switch (p->p(st ? st->lexcomp : tokStackEmpty,
                 bf ? bf->lexcomp : tokStackEmpty)) {
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
        p->reduce(p);
        break;

      default:
      case E0:
        fprintf(stderr, "syntactic error: stack [%d:%s], buffer [%d:%s]\n",
                st->lexcomp, st->lexem, bf->lexcomp, bf->lexem);
        done = 1; break;
      case E1:
        fprintf(stderr, "syntactic error: non-variable assignment\n");
        done = 1; break;
      case E2:
        fprintf(stderr, "syntactic error: wrong argument type for operator\n");
        done = 1; break;
      case E3:
        fprintf(stderr, "syntactic error: expected binary operator or eol\n");
        done = 1; break;
      case E4:
        fprintf(stderr, "syntactic error: unbalanced open parenthesis\n");
        done = 1; break;
      case E5:
        fprintf(stderr, "syntactic error: comma only allowed bt function arguments\n");
        done = 1; break;
      case E6:
        fprintf(stderr, "syntactic error: unbalanced closing parenthesis\n");
        done = 1; break;
      case E7:
        done = 1; break;
    }
  }

  double r = 0.0;
  symbol_t *ss;
  if ((ss = (symbol_t*)list_pop(p->partial))) {
    r = lookup_symbol(p->symbol_table, ss);
    symbol_destroy(ss);
    double *ans;
    if ((ans = (double*)hashtbl_get(p->symbol_table, "ans"))) {
      *ans = r;
    } else {
      ans = malloc(sizeof(double));
      *ans = r;
      hashtbl_insert(p->symbol_table, "ans", ans);
    }
  }

  list_destroy(p->partial);
  list_destroy(p->stack);
  return r;
}

/* vim: set sw=2 sts=2 : */
