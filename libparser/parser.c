#include <stdio.h>
#include <stdlib.h>

#include "parser-priv.h"

expr_t * parser_compile(scanner_t *s) {
  if (!s) {
#ifdef _VERBOSE_
    fprintf(stderr, "parser: Invalid scanner\n");
#endif
    return NULL;
  }

  list_t *stack = list_init((free_func_t)token_destroy, NULL);
  list_push(stack, token_init(tokStackEmpty, ""));
  list_t *partial = list_init(free, NULL);
  token_t *st, *bf = adjust_token(lexer_nextitem(s), NULL);

  int error = 0;
  while (error == 0) {
    st = (token_t*)list_peek_head(stack);
    switch (parser_precedence(st->lexcomp, bf->lexcomp)) {
      case LT:
        list_push(stack, token_init(tokOMango, ""));
        list_push(stack, bf);
        bf = adjust_token(lexer_nextitem(s), bf);
        break;
      case EQ:
        list_push(stack, token_init(tokEMango, ""));
        list_push(stack, bf);
        bf = adjust_token(lexer_nextitem(s), bf);
        break;
      case GT:
        error = semanter_reduce(stack, partial);
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
  list_destroy(stack);
  if (error > 0) {
    list_destroy(partial);
    return NULL;
  }
  return (expr_t*)partial;
}


void parser_destroy_expr(expr_t *e) {
  list_destroy((list_t*)e);
}

/* vim: set sw=2 sts=2 : */
