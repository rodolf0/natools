/* This parser is capable of parsing the following language:
 *
 * E -> number | variable
 * E -> variable=E
 * E -> E+E | E-E | -E | E*E | E/E | E^E | (E) | f(E, ...)
 *
 */
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

/* symbol table is the only list that remains alloc'ed accross multiple
 * evaluations. other elements are alloc'ed and free'd to avoid
 * carrying errors */
parser_t *parser_create() {
  parser_t *p = (parser_t*)malloc(sizeof(parser_t));

  p->operator_precedence = &_op_prec;
  p->symbol_table = list_init((void(*)(void*))token_destroy,
                      (int(*)(const void*, const void*))token_strcmp);

  p->parser_stack = NULL;
  p->result_stack = NULL;
  p->tokenlist = NULL;

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
  if (p->tokenlist)
    list_destroy(p->tokenlist);
  /* clean up operator_precedence table if changed */
  free(p);
}

/* this functions free's some resources for use after errors
 * and avoid free'ing the symbol table */
static void parser_reset(parser_t *p) {
  if (!p)
    return;
  if (p->parser_stack) {
    list_destroy(p->parser_stack);
    p->parser_stack = NULL;
  }
  if (p->result_stack) {
    list_destroy(p->result_stack);
    p->result_stack = NULL;
  }
  if (p->tokenlist) {
    list_destroy(p->tokenlist);
    p->tokenlist = NULL;
  }
}


#ifdef _ERROR_CHECKING_
static int parser_error_check(parser_t *p) {
#define tokenLComp(lnode) ((token_t*)lnode->data)->lcomp
#define tokenLexem(lnode) ((token_t*)lnode->data)->lexem

  /* this is the token just pushed to the stack */
  list_node_t *ps_top = p->parser_stack->first;
  /* this is the next token from the buffer */
  list_node_t *buf_fst = p->tokenlist->first;

  int func_params = 0;

  /* error checking */
  switch (tokenLComp(ps_top)) {
    /* non terminal reductions: shouldn't have non terminals on their sides */
    case number:
    case variable:
      if (tokenLComp(ps_top->next) != mango_o ||
          tokenLComp(ps_top->next->next) == number ||
          tokenLComp(ps_top->next->next) == variable ||
          tokenLComp(ps_top->next->next) == paren_close) {
        fprintf(stderr, "Syntactic error: missing operator before [%s].\n",
                        tokenLexem(ps_top));
        return 1;

      } else if (tokenLComp(buf_fst) == number ||
                 tokenLComp(buf_fst) == function ||
                 tokenLComp(buf_fst) == variable ||
                 tokenLComp(buf_fst) == paren_open) {
        fprintf(stderr, "Syntactic error: missing operator after [%s].\n",
                        tokenLexem(ps_top));
        return 1;
      }
      break;

    /* terminal reductions: should have non terminals on both sides */
    case op_add:
    case op_sub:
    case op_mul:
    case op_div:
    case op_pow:
    case op_asig:
      if (p->result_stack->size < 2) {
        fprintf(stderr, "Syntactic error: missing operand for [%s].\n",
                tokenLexem(ps_top));
        return 2;
      /* check that we only assign to a variable */
      } else if (tokenLComp(ps_top) == op_asig &&
                 tokenLComp(p->result_stack->first->next) != variable) {
        fprintf(stderr, "Syntactic error: missing operand for [%s]. "
                        "Assigning to a variable?\n",
                tokenLexem(ps_top));
        return 3;
      }
      break;

    case op_neg:
      if (p->result_stack->size < 1) {
        fprintf(stderr, "Syntactic error: missing operand for [%s].\n",
                tokenLexem(ps_top));
        return 2;
      }
      break;


    /* check for expresion inside () */
    case paren_close: /* ps_top = paren_close */
      /* check how many elements should the result stack have */
      for (ps_top = ps_top->next;
           ps_top &&
             tokenLComp(ps_top) != paren_open &&
             tokenLComp(ps_top) != function;
           ps_top = ps_top->next) {

        if (tokenLComp(ps_top) == mango_e)
          func_params++;
      }
      /* at this point ps_top either contains function or paren_open */
      if (p->result_stack->size < func_params) {
        /* if this isn't a 0 param function then we're missing an operand */
        if (tokenLComp(ps_top) != function ||
            strcmp("random(", tokenLexem(ps_top))) {

          fprintf(stderr, "Syntactic error: missing expresion / "
                          "argument inside ().\n");
          return 4;
        }
      }
      break;

    default:
      fprintf(stderr, "lex-comp [%d] can't make reduction at the top "
              "of the parser stack. No GT relation.", tokenLComp(ps_top));
      return 5;
      break;
  }
  return 0;

#undef tokenLComp
#undef tokenLexem
}
#endif /* _ERROR_CHECKING_ */

/* parser reduce: add semantic value if right hand production found */
static int parser_semantics_eval(parser_t *p) {
  token_t *t, *a, *b, *c;
  int func_params = 0;

  /* harves mango (starts with mango_o -> mango_c */
  while ((t = list_pop(p->parser_stack))->lcomp != mango_o) {

    a = b = c = NULL;

    /* pop equal mangos */
    if (t->lcomp == mango_e) {
      /* each mango_e may represent a function parameter if inside
       * a function. if func_params == 1 the function may take no
       * parameters too (eg random). This should complement with
       * error checking to validate */
      func_params++;

      /* destroy mango */
      token_destroy(t);
      continue;
    }


    /* add semantic value based on the lexical component */
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


      /* parenthesis n comma don't have semantic value */
      case paren_open:
      case paren_close:
      case op_comma:
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
          fprintf(stderr,
                  "Semantic error: variable [%s] not in symbol table.\n",
                  a->lexem);
          /* cleanup */
          token_destroy(a);
          token_destroy(b);
          token_destroy(t);
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
            break;
        }
        token_destroy(a); /* if NULL will be a nop */
        token_destroy(b);
        list_push(p->result_stack, t);
        break;


      case function:

        if (!strcmp("sin(", t->lexem)) {
          a = list_pop(p->result_stack);
          t->value = sin(a->value);
          token_destroy(a);

        } else if (!strcmp("cos(", t->lexem)) {
          a = list_pop(p->result_stack);
          t->value = cos(a->value);
          token_destroy(a);

        } else if (!strcmp("tan(", t->lexem)) {
          a = list_pop(p->result_stack);
          t->value = tan(a->value);
          token_destroy(a);

        } else if (!strcmp("asin(", t->lexem)) {
          a = list_pop(p->result_stack);
          t->value = asin(a->value);
          token_destroy(a);

        } else if (!strcmp("acos(", t->lexem)) {
          a = list_pop(p->result_stack);
          t->value = acos(a->value);
          token_destroy(a);

        } else if (!strcmp("atan(", t->lexem)) {
          a = list_pop(p->result_stack);
          t->value = atan(a->value);
          token_destroy(a);

        } else if (!strcmp("log(", t->lexem)) {
          a = list_pop(p->result_stack);
          t->value = log(a->value);
          token_destroy(a);

        } else if (!strcmp("abs(", t->lexem)) {
          a = list_pop(p->result_stack);
          t->value = fabs(a->value);
          token_destroy(a);

        } else if (!strcmp("random(", t->lexem)) {
          t->value = (double)random() / (double)RAND_MAX;

        } else if (!strcmp("max(", t->lexem)) {
          a = list_pop(p->result_stack);
          t->value = a->value;
          while (--func_params) {
            a = list_pop(p->result_stack);
            if (t->value < a->value)
              t->value = a->value;
            token_destroy(a);
          }

        } else if (!strcmp("min(", t->lexem)) {
          a = list_pop(p->result_stack);
          t->value = a->value;
          token_destroy(a);
          while (--func_params) {
            a = list_pop(p->result_stack);
            if (t->value > a->value)
              t->value = a->value;
            token_destroy(a);
          }

        } else if (!strcmp("sum(", t->lexem)) {
          t->value = 0.0;
          while (func_params--) {
            a = list_pop(p->result_stack);
            t->value += a->value;
            token_destroy(a);
          }

        } else {
          fprintf(stderr, "Semantic error: lexem [%s] not recognized.\n",
              t->lexem);
          return 3;
        }

        list_push(p->result_stack, t);
        break;


      default:
        fprintf(stderr, "Semantic error: unhandled lex-comp [%d].\n",
                t->lcomp);
        token_destroy(t);
        return 4;
        break;
    }
  }

  /* destroy opening mango */
  token_destroy(t);

  return 0;
}

int parser_eval(parser_t *p, const char* buf, double *ret) {

  if (!buf || buf[0] == '\0') {
    fprintf(stderr, "Can't evaluate empty expresion.\n");
    return 6;
  }

  /* tokenize the whole buffer (since tokens are queued
   * on the tail, we should pop them from the head to keep
   * the order (maybe fix the list function names ?)) */
  p->tokenlist = tokenize(buf);
  if (!p->tokenlist)
    return 1;

  p->parser_stack = list_init((void(*)(void*))token_destroy, NULL);
  p->result_stack = list_init((void(*)(void*))token_destroy, NULL);

  /* stack bottom place holder */
  token_t *t = token_init(stack_empty, "");
  list_push(p->parser_stack, t);

  token_t *stack_element, *buffr_element;

  for (stack_element = list_peek_head(p->parser_stack),
       buffr_element = list_peek_head(p->tokenlist);
       stack_element->lcomp != stack_empty ||
       buffr_element->lcomp != stack_empty;
       stack_element = list_peek_head(p->parser_stack),
       buffr_element = list_peek_head(p->tokenlist)
      ) {

    switch ((*p->operator_precedence)[stack_element->lcomp]
                                     [buffr_element->lcomp]) {
      case LT:
        t = token_init(mango_o, "");
        list_push(p->parser_stack, t);
        buffr_element = list_pop(p->tokenlist);
        list_push(p->parser_stack, buffr_element);
        break;

      case EQ:
        t = token_init(mango_e, "");
        list_push(p->parser_stack, t);
        buffr_element = list_pop(p->tokenlist);
        list_push(p->parser_stack, buffr_element);
        break;

      case GT:
#ifdef _ERROR_CHECKING_
        if (parser_error_check(p)) {
          parser_reset(p);
          return 2;
        }
#endif /* _ERROR_CHECKING_ */

        /* harvest mangos: idealy here we should insert to a parse tree,
         * instead use the implicit tree determined by mangos and the
         * result stack*/
        if (parser_semantics_eval(p)) {
          parser_reset(p);
          return 3;
        }
        break;

      default:
#ifdef _ERROR_CHECKING_
        parser_error_check(p);
#else
        fprintf(stderr, "Syntactic error: "
                  "buffer element[%s]=%d. stack element[%s]=%d\n",
                  stack_element->lexem, stack_element->lcomp,
                  buffr_element->lexem, buffr_element->lcomp);
#endif /* _ERROR_CHECKING_ */
        parser_reset(p);
        return 4;
        break;
    }
  }

  /* get result and de-allocate resources */
  t = list_pop(p->result_stack);

  /* keep special symbol 'ans' with previous result */
  token_t *sans, *ans = token_init(variable, "ans");
  if ((sans = list_find(p->symbol_table, ans))) {
    sans->value = t->value;
    token_destroy(ans);
  } else {
    ans->value = t->value;
    list_push(p->symbol_table, ans);
  }

  *ret = t->value;
  token_destroy(t);
  parser_reset(p);

  return 0;
}

/* vim: set sw=2 sts=2 : */
