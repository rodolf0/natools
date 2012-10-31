#include <stdlib.h>
#include <string.h>

#include "parser/lexer.h"

/* utility functions */
int is_num(char x) {
  return (x >= '0' && x <= '9');
}

int is_alpha(char x) {
  return (x >= 'a' && x <= 'z') ||
         (x >= 'A' && x <= 'Z') ||
         x == '_';
}

int is_white(char x) {
  return x == ' '  || x == '\t' ||
         x == '\n' || x == '\r';
}


static token_t * tok_maker(char *base, size_t n) {
  token_t *t = (token_t*)malloc(sizeof(token_t) + n + 1);
  t->lexem = (char*)t + sizeof(token_t); /* piggyback lexem */
  memcpy(t->lexem, base, n);
  t->lexem[n] = '\0';
  return t;
}

/* get the next token */
token_t * lexer_nextitem(scanner_t *s) {
  lexcomp_t lc;
  size_t i;

  lexcomp_t (*tokenizers[])(scanner_t*) = {
    tokenize_identifier,
    tokenize_text,
    tokenize_number,
    tokenize_mathops,
    tokenize_relops,
    tokenize_miscops,
  };

  /* consume all whitespace */
  while (is_white(scanner_advance(s)));
  scanner_backup(s);
  scanner_ignore(s);

  for (i = 0; i < sizeof(tokenizers)/sizeof(tokenizers[0]); i++) {
    if ((lc = tokenizers[i](s)) != tokNoMatch) {
      token_t *t = (token_t*)scanner_accept(s, (acceptfn)tok_maker);
      t->lexcomp = lc;
      return t;
    }
  }
  return NULL;
}

/* vim: set sw=2 sts=2 : */
