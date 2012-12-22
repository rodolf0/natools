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
    case tokTrue       : return 16;
    case tokFalse      : return 17;

    case tokEq         : return 18;
    case tokNe         : return 19;
    case tokGt         : return 20;
    case tokLt         : return 21;
    case tokGe         : return 22;
    case tokLe         : return 23;

    case tokOParen     : return 24;
    case tokCParen     : return 25;
    case tokComma      : return 26;
    case tokAsign      : return 27;

    case tokNumber     : return 28;
    case tokId         : return 29;
    case tokFunction   : return 30;
    case tokStackEmpty : return 31;
    /*case tokText       : return -1;*/
    default            : return -1;
  }
}

op_prec_t parser_precedence(lexcomp_t op1, lexcomp_t op2) {
  ssize_t row = decode_lexcomp(op1),
          col = decode_lexcomp(op2);
  if (row == -1 || col == -1)
    return E8;
  /* rows: element on the stack, cols: elements from the buffer */
  static const op_prec_t _precedence[32][32] = {
         /*  +   -   -u  *   /   %   **  >>  <<  &   |   ^   ~   !   &&  ||  tr  fa  ==  !=  >   <   >=  <=  (   )   ,   =   n   id  f   $  */
   /*  + */ {GT, GT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /*  - */ {GT, GT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /* -u */ {GT, GT, LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /*  * */ {GT, GT, LT, GT, GT, GT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /*  / */ {GT, GT, LT, GT, GT, GT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /*  % */ {GT, GT, LT, GT, GT, GT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /* ** */ {GT, GT, LT, GT, GT, GT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /* >> */ {LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /* << */ {LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /*  & */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, E1, LT, LT, LT, GT},
   /*  | */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, LT, LT, LT, GT, GT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, E1, LT, LT, LT, GT},
   /*  ^ */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, LT, LT, GT, GT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, E1, LT, LT, LT, GT},
   /*  ~ */ {GT, GT, LT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /*  ! */ {GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /* && */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, E1, LT, LT, LT, GT},
   /* || */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, E1, LT, LT, LT, GT},
   /* tr */ {GT, GT, E3, GT, GT, GT, GT, GT, GT, GT, GT, GT, E3, E3, GT, GT, E3, E3, GT, GT, GT, GT, GT, GT, E3, GT, GT, E1, E3, E3, E3, GT},
   /* fa */ {GT, GT, E3, GT, GT, GT, GT, GT, GT, GT, GT, GT, E3, E3, GT, GT, E3, E3, GT, GT, GT, GT, GT, GT, E3, GT, GT, E1, E3, E3, E3, GT},
   /* == */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, LT, LT, LT, LT, LT, GT, GT, E1, LT, LT, LT, GT},
   /* != */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, LT, LT, LT, LT, LT, GT, GT, E1, LT, LT, LT, GT},
   /*  > */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /*  < */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /* >= */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /* <= */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, GT, GT, GT, LT, LT, GT, GT, LT, LT, GT, GT, GT, GT, GT, GT, LT, GT, GT, E1, LT, LT, LT, GT},
   /*  ( */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, EQ, E5, E1, LT, LT, LT, E4},
   /*  ) */ {GT, GT, E3, GT, GT, GT, GT, GT, GT, GT, GT, GT, E3, E3, GT, GT, E3, E3, GT, GT, GT, GT, GT, GT, E3, GT, GT, E1, E3, E3, E3, GT},
   /*  , */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, EQ, EQ, E1, LT, LT, LT, E5},
   /*  = */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, E6, E5, E7, LT, LT, LT, GT},
   /*  n */ {GT, GT, E3, GT, GT, GT, GT, GT, GT, GT, GT, GT, E3, E3, GT, GT, E3, E3, GT, GT, GT, GT, GT, GT, E3, GT, GT, E1, E3, E3, E3, GT},
   /* id */ {GT, GT, E3, GT, GT, GT, GT, GT, GT, GT, GT, GT, E3, E3, GT, GT, E3, E3, GT, GT, GT, GT, GT, GT, E3, GT, GT, GT, E3, E3, E3, GT},
   /*  f */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, EQ, EQ, E1, LT, LT, LT, E4},
   /*  $ */ {LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, LT, E6, E5, LT, LT, LT, LT, E0},
  };
  return _precedence[row][col];
}


/* check if t-token is a unary-minus, return adjusted-t */
token_t * adjust_token(token_t *t, token_t *prev) {
  if (!t || t->lexcomp != tokMinus)
    return t;

  if (!prev ||
      /* math operators */
      prev->lexcomp == tokPlus ||
      prev->lexcomp == tokMinus ||
      prev->lexcomp == tokUnaryMinus ||
      prev->lexcomp == tokTimes ||
      prev->lexcomp == tokDivide ||
      prev->lexcomp == tokModulo ||
      prev->lexcomp == tokPower ||
      /* bitwise operators */
      prev->lexcomp == tokRShift ||
      prev->lexcomp == tokLShift ||
      prev->lexcomp == tokBitAnd ||
      prev->lexcomp == tokBitOr ||
      prev->lexcomp == tokBitXor ||
      prev->lexcomp == tokBitNot ||
      /* logical operators */
      prev->lexcomp == tokNot ||
      prev->lexcomp == tokAnd ||
      prev->lexcomp == tokOr ||
      /* relational operators */
      prev->lexcomp == tokEq ||
      prev->lexcomp == tokNe ||
      prev->lexcomp == tokGt ||
      prev->lexcomp == tokLt ||
      prev->lexcomp == tokGe ||
      prev->lexcomp == tokLe ||
      /* misc operators */
      prev->lexcomp == tokOParen ||
      prev->lexcomp == tokFunction ||
      prev->lexcomp == tokComma ||
      prev->lexcomp == tokAsign ||
      prev->lexcomp == tokStackEmpty)
    t->lexcomp = tokUnaryMinus;

  return t;
}

/* vim: set sw=2 sts=2 : */
