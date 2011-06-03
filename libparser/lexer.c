#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/lexer.h"

/* available lex-comp scanners that the tokenize function
 * will recognize */
lex_component scan_alphanums(scanner_t *s);
lex_component scan_numbers(scanner_t *s);
lex_component scan_operator(scanner_t *s);

lex_component (*comp_scanners[])(scanner_t*) = {
  scan_alphanums,
  scan_numbers,
  scan_operator
};

list_t *tokenize(const char *buffer) {
  scanner_t *scanner = scanner_init(buffer);
  list_t *tok_list = list_init((void(*)(void*))token_destroy, NULL);
  lex_component lcomp = no_match;
  token_t *token;
  size_t lexem_start, lexem_end, i;
  char tok_buf[256];

  while (scanner->next_char) {
    while (IS_WHITE(scanner->next_char)) {
      scanner_readchar(scanner);
    }

    lexem_start = scanner->pos + 1;

    for (i = 0; i < sizeof(comp_scanners)/sizeof(void*); i++) {
      if ((lcomp = comp_scanners[i](scanner)) != no_match)
        break;
      scanner_seek(scanner, lexem_start - 1);
    }

    if (lcomp == no_match) {
      fprintf(stderr, "Lexical error, can't recognize: %s at %d.\n",
              buffer + lexem_start, (int)lexem_start);
      list_destroy(tok_list);
      return NULL;
      break;
    } else {
      lexem_end = scanner->pos + 1;
      strncpy(tok_buf, scanner->buffer + lexem_start,
              lexem_end - lexem_start);
      tok_buf[lexem_end-lexem_start] = '\0';
      token = token_init(lcomp, tok_buf);

      list_queue(tok_list, token);
    }
  }

  scanner_destroy(scanner);

  /* placeholder for the buffer */
  token = token_init(stack_empty, "");
  list_queue(tok_list, token);

  return tok_list;
}

token_t *token_init(lex_component lc, const char *lexem) {
  token_t *t = (token_t*)malloc(sizeof(token_t));
  size_t len = strlen(lexem);

  t->lexem = (char*)malloc(len + 1);
  strncpy(t->lexem, lexem, len);
  t->lexem[len] = '\0';
  t->lcomp = lc;
  t->value = 0.0;

  return t;
}

void token_destroy(token_t *t) {
  if (t) {
    if (t->lexem)
      free(t->lexem);
    free(t);
  }
}

lex_component scan_alphanums(scanner_t *s) {
  int state = 0;

  while (1) {
    switch (state) {
      case 0:
        if (IS_ALPHA(s->next_char))
          state = 1;
        else
          return no_match;
        break;

      case 1:
        if (IS_ALPHA(s->next_char) ||
            IS_NUM(s->next_char))
          state = 1;
        else if (s->next_char == '(')
          state = 2;
        else
          return variable;
        break;

      case 2:
        return function;
        break;
    }
    scanner_readchar(s);
  }
  return no_match;
}

lex_component scan_numbers(scanner_t *s) {
  int state = 0;

  while (1) {
    switch (state) {
      case 0:
        if (IS_NUM(s->next_char))
          state = 1;
        else
          return no_match;
        break;

      /* integer part */
      case 1:
        if (IS_NUM(s->next_char))
          state = 1;
        else if (s->next_char == '.')
          state = 2;
        else if (s->next_char == 'E' ||
            s->next_char == 'e')
          state = 4;
        else
          return number;
        break;

      /* fractional part */
      case 2:
        if (IS_NUM(s->next_char))
          state = 3;
        else
          return no_match;
        break;

      case 3:
        if (IS_NUM(s->next_char))
          state = 3;
        else if (s->next_char == 'e' ||
                   s->next_char == 'E')
          state = 4;
        else
          return number;
        break;

      /* exponent sign */
      case 4:
        if (s->next_char == '-' ||
            s->next_char == '+')
          state = 5;
        else if (IS_NUM(s->next_char))
          state = 6;
        else
          return no_match;
        break;

      /* exponent */
      case 5: /* at least one digit */
        if (IS_NUM(s->next_char))
          state = 6;
        else
          return no_match;
        break;

      case 6:
        if (IS_NUM(s->next_char))
          state = 6;
        else
          return number;
        break;
    }
    scanner_readchar(s);
  }
  return no_match;
}

lex_component scan_operator(scanner_t *s) {
  lex_component ret;

  switch(s->next_char) {
    case '+': ret = op_add; break;
    case '-':
      if (MAKES_UNARYOP(s->current_char))
        ret = op_neg;
      else
        ret = op_sub;
      break;
    case '*': ret = op_mul; break;
    case '/': ret = op_div; break;
    case '^': ret = op_pow; break;
    case ',': ret = op_comma; break;
    case '(': ret = paren_open; break;
    case ')': ret = paren_close; break;
    case '=': ret = op_asig; break;
    default:
      return no_match;
  }
  scanner_readchar(s);
  return ret;
}

/* vim: set sw=2 sts=2 : */
