#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "parser.h"
#include "lexer.h"

/* operator precedence table */
#define LT -1
#define EQ 0
#define GT 1
#define ER 2

static const char _op_prec[14][14] = {
        /*  n   +   -   *   /   ^   (   )   $   f   ,   -u  v   = */
   /* n */ {ER, GT, GT, GT, GT, GT, ER, GT, GT, ER, GT, ER, ER, ER},
   /* + */ {LT, GT, GT, LT, LT, LT, LT, GT, GT, LT, GT, LT, LT, LT},
   /* - */ {LT, GT, GT, LT, LT, LT, LT, GT, GT, LT, GT, LT, LT, LT},
   /* * */ {LT, GT, GT, GT, GT, LT, LT, GT, GT, LT, GT, LT, LT, LT},
   /* / */ {LT, GT, GT, GT, GT, LT, LT, GT, GT, LT, GT, LT, LT, LT},
   /* ^ */ {LT, GT, GT, GT, GT, LT, LT, GT, GT, LT, GT, LT, LT, LT},
   /* ( */ {LT, LT, LT, LT, LT, LT, LT, EQ, ER, LT, ER, LT, LT, LT},
   /* ) */ {ER, GT, GT, GT, GT, GT, ER, GT, GT, ER, GT, ER, ER, ER},
   /* $ */ {LT, LT, LT, LT, LT, LT, LT, ER, ER, LT, ER, LT, LT, LT},
   /* f */ {LT, LT, LT, LT, LT, LT, LT, EQ, ER, LT, EQ, LT, LT, LT},
   /* , */ {LT, LT, LT, LT, LT, LT, LT, EQ, ER, LT, EQ, LT, LT, LT},
   /* -u*/ {LT, GT, GT, GT, GT, GT, LT, GT, GT, LT, GT, ER, LT, LT},
   /* v */ {ER, GT, GT, GT, GT, GT, ER, GT, GT, ER, GT, ER, ER, GT},
   /* = */ {LT, LT, LT, LT, LT, LT, LT, GT, GT, LT, GT, LT, LT, LT},
};

static int token_strcmp(const token_t *t1, const token_t *t2) {
  return strcmp(t1->lexem, t2->lexem);
}

parser_t *parser_create() {
  parser_t *p = (parser_t*)malloc(sizeof(parser_t));

  p->symbol_table = list_init((void(*)(void*))token_destroy, 
                      (int(*)(const void*, const void*))token_strcmp);
  p->parser_stack = list_init((void(*)(void*))token_destroy, NULL);
  p->result_stack = list_init((void(*)(void*))token_destroy, NULL);
  p->operator_precedence = &_op_prec;

  return p;
}

void parser_destroy(parser_t *p) {
  if (!p)
    return;
  if (p->parser_stack)
    list_destroy(p->parser_stack);
  if (p->result_stack)
    list_destroy(p->result_stack);
  if (p->symbol_table)
    list_destroy(p->symbol_table);
  /* clean up operator_precedence table if changed */
  free(p);
}

static int parser_semantics_eval(parser_t *p) {

  token_t *t, *a, *b, *c;
  list_t *l = list_init((void(*)(void*))token_destroy, NULL);

  while ((t = list_pop(p->parser_stack))->lcomp != mango_o) {

    /* harvest arguments for functions from result stack */
    if (t->lcomp == mango_e) {
      a = list_pop(p->result_stack);
      list_push(l, a);
      /* destroy mango */
      token_destroy(t);
      continue;
    }

    a = b = c = NULL;

    switch (t->lcomp) {
      case number:
        t->value = atof(t->lexem);
        list_push(p->result_stack, t);
        break;

      case variable:
        if (!strcmp("pi", t->lexem))
          t->value = M_PI;
        else if (!strcmp("e", t->lexem))
          t->value = M_E;
        else {
          /* find a previous instance of the variable or allocate it */
          if (!(a = (token_t*)list_find(p->symbol_table, t))) {
            a = token_init(variable, t->lexem);
            list_push(p->symbol_table, a);
          }
          t->value = a->value;
        }
        list_push(p->result_stack, t);
        break;

      /* parenthesis don't have semantic value */
      case paren_open:
      case paren_close:
        token_destroy(t);
        break;

      /* asign to a variable */
      case op_asig:
        /* b holds the value that will be assigned */
        b = list_pop(p->result_stack);
        /* a holds the variable name to which we'll assign */
        a = list_pop(p->result_stack);
        /* find the variable in the symbol table */
        if (!(c = (token_t*)list_find(p->symbol_table, a))) {
          fprintf(stderr, "Semantic error: %s not in symbol table.\n",
                          a->lexem);
          return 5;
        }
        /* store the new value in the symbol table */
        t->value = c->value = b->value;

        token_destroy(a);
        token_destroy(b);
        list_push(p->result_stack, t);
        break;

      case op_add:
      case op_sub:
      case op_mul:
      case op_div:
      case op_pow:
        b = list_pop(p->result_stack);
      case op_neg:
        a = list_pop(p->result_stack);
        
        switch (t->lcomp) {
          case op_add:
            t->value = a->value + b->value;
            break;
          case op_sub:
            t->value = a->value - b->value;
            break;
          case op_mul:
            t->value = a->value * b->value;
            break;
          case op_div:
            t->value = a->value / b->value;
            break;
          case op_pow:
            t->value = pow(a->value, b->value);
            break;
          case op_neg:
            t->value = -a->value;
            break;
          default:
            fprintf(stderr, "Semantic error: expected an operator.\n");
            return 2;
            break;
        }
        token_destroy(a); /* if NULL will be a nop */
        token_destroy(b);
        list_push(p->result_stack, t);
        break;

      case function:
        if (l->size == 1) {
          a = list_pop(l);
          t->value = a->value;
          token_destroy(a);

          if (!strcmp("sin(", t->lexem))
            t->value = sin(t->value);
          else if (!strcmp("cos(", t->lexem))
            t->value = cos(t->value);
          else if (!strcmp("tan(", t->lexem))
            t->value = tan(t->value);
          else if (!strcmp("asin(", t->lexem))
            t->value = asin(t->value);
          else if (!strcmp("acos(", t->lexem))
            t->value = acos(t->value);
          else if (!strcmp("atan(", t->lexem))
            t->value = atan(t->value);
          else if (!strcmp("log(", t->lexem))
            t->value = log(t->value);
          else if (!strcmp("abs(", t->lexem))
            t->value = fabs(t->value);
        } else if (l->size == 0) {
          /* random */ ;
        } else if (l->size > 1) {
          ;
        } else {
          /* TODO: free lists, token t */
          fprintf(stderr, "Semantic error: %s not recognized.\n", t->lexem);
          return 3;
        }
        list_push(p->result_stack, t);
        break;

      default:
        fprintf(stderr, "Semantic error: unexpected lex-comp.\n");
        return 4;
        break;
    }
  }
  /* destroy opening mango */
  token_destroy(t);
  /* pop back to the result stack unused elements */
  while (l->size)
    list_push(p->result_stack, list_pop(l));
  list_destroy(l);

  return 0;
}

int parser_eval(parser_t *p, const char* buf, double *ret) {

  /* stack bottom place holder */
  token_t *t = token_init(stack_empty, "");
  list_push(p->parser_stack, t);

  /* tokenize the whole buffer (since tokens are queued
   * on the tail, we should pop them from the head to keep
   * the order (maybe fix the list function names ?)) */
  list_t *tokenlist = tokenize(buf);
  if (!tokenlist)
    return 1;

  token_t *stack_element, *buffr_element;

  for (stack_element = (token_t*)p->parser_stack->first->data,
       buffr_element = (token_t*)tokenlist->first->data;
       stack_element->lcomp != stack_empty ||
       buffr_element->lcomp != stack_empty;
       stack_element = (token_t*)p->parser_stack->first->data,
       buffr_element = (token_t*)tokenlist->first->data
      ) {

    switch ((*p->operator_precedence)[stack_element->lcomp]
                                     [buffr_element->lcomp]) {
      case LT:
        t = token_init(mango_o, "");
        list_push(p->parser_stack, t);
        buffr_element = list_pop(tokenlist);
        list_push(p->parser_stack, buffr_element);
        break;

      case EQ:
        t = token_init(mango_e, "");
        list_push(p->parser_stack, t);
        buffr_element = list_pop(tokenlist);
        list_push(p->parser_stack, buffr_element);
        break;

      case GT:
        /* harvest mangos: idealy here we should insert to a parse tree,
         * instead use the implicit tree determined by mangos and the
         * result stack*/
        if (parser_semantics_eval(p))
          return 1;
        break;

      default:
        fprintf(stderr, "Syntactic error: "
                  "buffer element(%s)=%d. stack element(%s)=%d\n",
                  stack_element->lexem, stack_element->lcomp,
                  buffr_element->lexem, buffr_element->lcomp);
        return 1;
        break;
    }
  }
  *ret = ((token_t*)p->result_stack->first->data)->value;
  return 0;
}

/* vim: set sw=2 sts=2 : */
