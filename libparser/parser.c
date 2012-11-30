#include <stdio.h>
#include "baas/list.h"
#include "parser/parser.h"

/* parser-math specific */
extern token_t * adjust_minus(token_t *t, token_t *prev);


parser_t * parser_create(precedencefn pfn) {
  if (!pfn)
    return NULL;
  parser_t *p = (parser_t*)malloc(sizeof(parser_t));
  p->p = pfn;
  p->symbol_table = hashtbl_init((free_func_t)symbol_destroy, NULL);
  return p;
}

void parser_destroy(parser_t *p) {
  if (!p)
    return;
  list_destroy(p->symbol_table);
  free(p);
}



int parser_eval(parser_t *p, scanner_t *s) {
  if (!p || !s)
    return NULL;

  p->stack = list_init((free_func_t)token_destroy, NULL);
  list_push(p->stack, token_init(tokStackEmpty, ""));
  token_t *st = (token_t*)list_peek_head(p->stack);
  token_t *bf = adjust_minus(lexer_nextitem(s), NULL);

  /* partial result stack keeps elements to build expressions */
  p->partial = list_init(NULL, NULL);

  int done = 0;
  while (st && bf && !done) {
    switch (p->p(st->lexcomp, bf->lexcomp)) {
      case LT:
        list_push(p->stack, token_init(tokOMango, ""));
        list_push(p->stack, bf);
        break;

      case EQ:
        list_push(p->stack, token_init(tokEMango, ""));
        list_push(p->stack, bf);
        break;

      case GT:
        parser_semantic_eval(p);
        break;

      case E7:
        done = 1;
        break;

      default:
        fprintf(stderr, "syntactic error: stack [%d:%s], buffer [%d:%s]\n",
                s->lexcomp, s->lexem, b->lexcomp, b->lexem);
        expr = NULL;
        /* TODO: cleanup */
        break;
    }
    st = (token_t*)list_peek_head(p->stack);
    bf = adjust_minus(lexer_nextitem(s), bf);
  }
  /* clean up */
  list_push(stack, b);
  list_destroy(stack);
  return (expresion_t*)r;
}


double * parser_eval(parser_t *p, expresion_t *e) {
}

/* vim: set sw=2 sts=2 : */
