#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "baas/list.h"
#include "parser/scanner.h"
#include "parser/lexer.h"

struct lexer_t {
  scanner_t *s;
  list_t *tokenized;
  list_node_t *curtok;
};

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


static token_t * tok_maker(const char *base, size_t n) {
  token_t *t = (token_t*)zmalloc(sizeof(token_t) + n + 1);
  t->lexem = (char*)t + sizeof(token_t); /* piggyback lexem */
  memmove(t->lexem, base, n);
  t->lexem[n] = '\0';
  return t;
}

token_t * token_init(lexcomp_t lc, const char *lexem) {
  token_t *t = tok_maker(lexem, strlen(lexem));
  t->lexcomp = lc;
  return t;
}

void token_destroy(token_t *t) {
  if (!t)
    return;
  free(t);
}


/* known tokenizers */
lexcomp_t tokenize_identifier(scanner_t *s);
lexcomp_t tokenize_text(scanner_t *s);
lexcomp_t tokenize_number(scanner_t *s);
lexcomp_t tokenize_mathops(scanner_t *s);
lexcomp_t tokenize_relops(scanner_t *s);
lexcomp_t tokenize_bitops(scanner_t *s);
lexcomp_t tokenize_miscops(scanner_t *s);

/* get the next token out of a scanner */
token_t * lexer_nextitem(scanner_t *s) {
  /* try to match longest tokens first */
  static lexcomp_t (*tokenizers[])(scanner_t*) = {
    tokenize_text,
    tokenize_identifier,
    tokenize_number,
    tokenize_bitops,
    tokenize_relops,
    tokenize_mathops,
    tokenize_miscops,
  };

  lexcomp_t lc;
  size_t i;

  /* consume all whitespace */
  while (is_white(scanner_advance(s)));
  scanner_backup(s);
  scanner_ignore(s);

  if (scanner_peek(s) == 0)
    return token_init(tokStackEmpty, "");

  for (i = 0; i < sizeof(tokenizers)/sizeof(tokenizers[0]); i++) {
    if ((lc = tokenizers[i](s)) != tokNoMatch) {
      token_t *t = (token_t*)scanner_accept(s, (acceptfn)tok_maker);
      t->lexcomp = lc;
      return t;
    }
  }

  return token_init(tokNoMatch, "");
}


lexer_t * lexer_init(scanner_t *s) {
  if (!s)
    return NULL;
  lexer_t *l = (lexer_t*)zmalloc(sizeof(lexer_t));
  l->s = s;
  l->tokenized = list_init((free_func_t)token_destroy, NULL);
  l->curtok = NULL;
  return l;
}

void lexer_destroy(lexer_t *l) {
  if (!l)
    return;
  list_destroy(l->tokenized);
  free(l);
}

token_t * lexer_advance(lexer_t *l) {
  if (!l)
    return NULL;
  /* ingest data */
  if (!l->curtok) {
    if (list_size(l->tokenized) == 0)
      list_queue(l->tokenized, lexer_nextitem(l->s));
    l->curtok = list_first(l->tokenized);
    return (token_t*)list_data(l->curtok);
  }
  if (!list_next(l->curtok))
    list_queue(l->tokenized, lexer_nextitem(l->s));
  l->curtok = list_next(l->curtok);
  return (token_t*)list_data(l->curtok);
}

token_t * lexer_peek(lexer_t *l) {
  if (!l)
    return NULL;
  list_node_t *n = l->curtok;
  token_t *t = lexer_advance(l);
  l->curtok = n;
  return t;
}

token_t * lexer_current(lexer_t *l) {
  if (!l)
    return NULL;
  return (token_t*)list_data(l->curtok);
}

token_t * lexer_backup(lexer_t *l) {
  if (!l || !l->curtok)
    return NULL;
  l->curtok = list_prev(l->curtok);
  if (l->curtok)
    return (token_t*)list_data(l->curtok);
  return NULL;
}

/* drop already scanned tokens without destructing them */
void lexer_shift(lexer_t *l) {
  if (!l || !l->curtok)
    return;
  while (list_first(l->tokenized) != l->curtok)
    list_pop(l->tokenized);
  list_pop(l->tokenized);
  l->curtok = NULL;
}

/* like shift but free all used tokens */
void lexer_consume(lexer_t *l) {
  if (!l || !l->curtok)
    return;
  while (list_first(l->tokenized) != l->curtok)
    token_destroy((token_t*)list_pop(l->tokenized));
  token_destroy((token_t*)list_pop(l->tokenized));
  l->curtok = NULL;
}

/* vim: set sw=2 sts=2 : */
