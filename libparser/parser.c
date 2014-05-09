#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "parser-priv.h"


/* map a lexical component type to an index in the precedence matrix */
static ssize_t decode_lexcomp(lexcomp_t lc) {
  switch (lc) {
    case tokPlus       : return 0;
    case tokMinus      : return 1;
    case tokUnaryMinus : return 2;
    case tokTimes      : return 3;
    case tokDivide     : return 4;
    case tokModulo     : return 5;
    case tokPower      : return 6;

    case tokRShift     : return 7;
    case tokLShift     : return 8;
    case tokBitAnd     : return 9;
    case tokBitOr      : return 10;
    case tokBitXor     : return 11;
    case tokBitNot     : return 12;

    case tokNot        : return 13;
    case tokAnd        : return 14;
    case tokOr         : return 15;

    case tokEq         : return 16;
    case tokNe         : return 17;
    case tokGt         : return 18;
    case tokLt         : return 19;
    case tokGe         : return 20;
    case tokLe         : return 21;

    case tokOParen     : return 22;
    case tokCParen     : return 23;
    case tokComma      : return 24;

    case tokTrue       : return 25;
    case tokFalse      : return 25;
    case tokNumber     : return 25;
    case tokId         : return 25;
    case tokFunction   : return 26;
    case tokStackEmpty : return 27;

    case tokText: case tokAsign: case tokNoMatch:
    case tokOMango: case tokEMango: case tokCMango:
      return -1;
  }
  return -1;
}

op_prec_t parser_precedence(lexcomp_t op1, lexcomp_t op2) {
  ssize_t row = decode_lexcomp(op1),
          col = decode_lexcomp(op2);
  if (row == -1 || col == -1)
    return E8;
  /* rows: element on the stack, cols: elements from the buffer */
  static const op_prec_t _precedence[28][28] = {
         /*  +   -   -u  *   /   %   **  >>  <<  &   |   ^   ~   !   &&  ||  ==  !=  >   <   >=  <=  (   )   ,   id  f   $  */
   /*  + */ {GT, GT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /*  - */ {GT, GT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /* -u */ {GT, GT, LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /*  * */ {GT, GT, LT, GT, GT, GT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /*  / */ {GT, GT, LT, GT, GT, GT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /*  % */ {GT, GT, LT, GT, GT, GT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /* ** */ {GT, GT, LT, GT, GT, GT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /* >> */ {LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /* << */ {LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /*  & */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, LT, LT, LT, LT, LT, LT, LT, GT, GT, LT, LT, GT},
   /*  | */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, LT, LT, LT, GT, GT, LT, LT, LT, LT, LT, LT, LT, GT, GT, LT, LT, GT},
   /*  ^ */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, LT, LT, GT, GT, LT, LT, LT, LT, LT, LT, LT, GT, GT, LT, LT, GT},
   /*  ~ */ {GT, GT, LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /*  ! */ {GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /* && */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, LT, LT, LT, LT, LT, LT, LT, GT, GT, LT, LT, GT},
   /* || */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, LT, LT, LT, LT, LT, LT, LT, GT, GT, LT, LT, GT},
   /* == */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, GT, GT, LT, LT, LT, LT, LT, GT, GT, LT, LT, GT},
   /* != */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, GT, GT, LT, LT, LT, LT, LT, GT, GT, LT, LT, GT},
   /*  > */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /*  < */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /* >= */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /* <= */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, GT, GT, LT, GT, GT, LT, LT, GT},
   /*  ( */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, EQ, E5, LT, LT, E4},
   /*  ) */ {GT, GT, E3, GT, GT, GT, GT, GT, GT, GT, GT, GT, E3, E3, GT, GT, GT, GT, GT, GT, GT, GT, E3, GT, GT, E3, E3, GT},
   /*  , */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, EQ, EQ, LT, LT, E5},
   /* id */ {GT, GT, E3, GT, GT, GT, GT, GT, GT, GT, GT, GT, E3, E3, GT, GT, GT, GT, GT, GT, GT, GT, E3, GT, GT, E3, E3, GT},
   /*  f */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, EQ, EQ, LT, LT, E4},
   /*  $ */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, E6, E5, LT, LT, E0},
  };
  return _precedence[row][col];
}


/* check if t-token is a unary-minus, return adjusted-t */
token_t * adjust_token(token_t *t, token_t *prev) {
  if (!t || t->lexcomp != tokMinus)
    return t;

  if (!prev)
    t->lexcomp = tokUnaryMinus;
  else
    switch (prev->lexcomp) {
      /* math operators */
      case tokPlus  : case tokMinus  : case tokUnaryMinus :
      case tokTimes : case tokDivide : case tokModulo     : case tokPower :
      /* bitwise operators */
      case tokRShift : case tokLShift : case tokBitAnd :
      case tokBitOr  : case tokBitXor : case tokBitNot :
      /* logical operators */
      case tokNot : case tokAnd : case tokOr :
      /* relational operators */
      case tokEq : case tokNe : case tokGt :
      case tokLt : case tokGe : case tokLe :
      /* misc operators */
      case tokOParen     : case tokFunction :
      case tokComma      : case tokAsign    :
      case tokStackEmpty :
        t->lexcomp = tokUnaryMinus;
        break;

      /* list so compiler warns us on missing case */
      case tokTrue: case tokFalse: case tokNumber: case tokText: case tokId:
      case tokOMango: case tokEMango: case tokCMango:
      case tokCParen: case tokNoMatch:
        break;
    }

  return t;
}


expr_t * parser_compile(lexer_t *l) {
  if (!l) {
    return NULL;
  }

  list_t *stack = list_init(NULL, NULL),
         *partial = list_init(free, NULL),
         *track= list_init((free_func_t)token_destroy, NULL);
  token_t *st = token_init(tokStackEmpty, ""),
          *bf = adjust_token(lexer_advance(l), NULL);

  list_push(stack, st); /* initialize the stack to the empty token */
  list_push(track, st);

  int error = 0;
  op_prec_t p;
  while (error == 0) {
    st = (token_t*)list_peek_head(stack);
    switch ((p = parser_precedence(st->lexcomp, bf->lexcomp))) {
      case LT:
      case EQ:
        st = token_init((p == LT) ? tokOMango : tokEMango, "");
        list_push(stack, st); list_push(track, st);
        list_push(stack, bf);
        bf = adjust_token(lexer_advance(l), bf);
        break;
      case GT:
        error = semanter_reduce(stack, partial);
        break;

      case E0:
        error = -1; break; /* parsing finished */
      case E3:
        fprintf(stderr, "syntactic error: expected binary operator or eol\n");
        error = 3; break;
      case E4:
        fprintf(stderr, "syntactic error: unbalanced open parenthesis\n");
        error = 4; break;
      case E5:
        fprintf(stderr, "syntactic error: comma only allowed bt function arguments\n");
        error = 5; break;
      case E6:
        fprintf(stderr, "syntactic error: unbalanced closing parenthesis\n");
        error = 6; break;
      case E8:
        fprintf(stderr, "syntactic error: stack [%d:%s], buffer [%d:%s]\n",
                st->lexcomp, st->lexem, bf->lexcomp, bf->lexem);
        error = 8; break;
    }
  }

  list_destroy(stack);
  list_destroy(track);
  if (error > 0) {
    list_destroy(partial);
    return NULL;
  }
  return (expr_t*)partial;
}


void parser_destroy_expr(expr_t *e) {
  list_destroy((list_t*)e);
}


/* wrapper functions to avoid constructing everything */
expr_t * parser_compile_str(const char *str) {
  scanner_t *s = scanner_init(str);
  lexer_t *l = lexer_init(s);
  expr_t *e = parser_compile(l);
  lexer_destroy(l);
  scanner_destroy(s);
  return e;
}

#define unlikely(x) __builtin_expect(!!(x), 0)
long double parser_qeval(const char *expr) {
  static hashtbl_t *vars = NULL;
  if (unlikely(vars == NULL))
    vars = hashtbl_init(free, NULL);
  if (unlikely(vars && !strcmp(expr, "_shutdown"))) {
    hashtbl_destroy(vars);
    return 0.0;
  }
  long double r = 0.0;
  expr_t *e = parser_compile_str(expr);
  parser_eval(e, &r, vars);
  parser_destroy_expr(e);
  return r;
}

/* vim: set sw=2 sts=2 : */
