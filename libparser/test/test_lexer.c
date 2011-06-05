#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../parser/lexer.h"

#define MAKES_UNARYOP(c) (c=='+' || c=='-' || c=='*' || \
                          c=='/' || c=='^' || c==',' || \
                          c=='(' || c=='=' || c=='\0' ? 1:0)

list_t * generate_random_buffer(int length) {
  list_t *r = list_init(free, NULL);
  char buf[64], *c;
  int i;

  while (length--) {
    switch (random() % (op_asig + 1)) {
      case number:
        /* avoid numbers following variables */
        if ((r->last && ((token_t*)r->last->data)->lcomp != variable) &&
            (r->last && ((token_t*)r->last->data)->lcomp != number)) {
          snprintf(buf, sizeof(buf), "%f", 100.0*random()/random());
          list_queue(r, token_init(number, buf));
          length++;
        }
        break;
      case op_add:
        list_queue(r, token_init(op_add, "+"));
        break;
      case op_sub:
        if (r->last) {
          c = ((token_t*)r->last->data)->lexem;
          if (c && !MAKES_UNARYOP(c[strlen(c)-1]))
            list_queue(r, token_init(op_sub, "-"));
          else
            length++;
        } else
          length++;
        break;
      case op_mul:
        list_queue(r, token_init(op_mul, "*"));
        break;
      case op_div:
        list_queue(r, token_init(op_div, "/"));
        break;
      case op_pow:
        list_queue(r, token_init(op_pow, "^"));
        break;
      case paren_open:
        if (r->last && ((token_t*)r->last->data)->lcomp != variable)
          list_queue(r, token_init(paren_open, "("));
        else
          length++;
        break;
      case paren_close:
        list_queue(r, token_init(paren_close, ")"));
        break;
      case stack_empty:
        break;
      case function:
        if (r->last && ((token_t*)r->last->data)->lcomp != variable) {
          snprintf(buf, sizeof(buf), "%c%c%c(",
                   (int)('f' + random() % ('z' - 'f')),
                   (int)('f' + random() % ('z' - 'f')),
                   (int)('f' + random() % ('z' - 'f')));
          list_queue(r, token_init(function, buf));
        } else
          length++;
        break;
      case op_comma:
        list_queue(r, token_init(op_comma, ","));
        break;
      case op_neg:
        if (r->last && MAKES_UNARYOP(((token_t*)r->last->data)->lexem[0]))
          list_queue(r, token_init(op_neg, "-"));
        else
          length++;
        break;
      case variable:
        if (r->last && ((token_t*)r->last->data)->lcomp != variable) {
          memset(buf, 0, sizeof(buf));
          char *alpha = "abcdfghijklmnopqrstuvwxyz";
          for (i = 0; i < 5; i++)
            strncat(buf, &alpha[random() % strlen(alpha)], 1);
          list_queue(r, token_init(variable, buf));
        } else
          length++;
        break;
      case op_asig:
        list_queue(r, token_init(op_asig, "="));
        break;
    }
  }

  list_queue(r, token_init(stack_empty, ""));
  return r;
}

void test_lexer() {
  list_t *b = generate_random_buffer(random() % 100);
  list_node_t *it, *it2;

  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  for (it = b->first; it; it = it->next)
    strcat(buffer, (char*)((token_t*)it->data)->lexem);

  /*fprintf(stderr, "   lx: %s\n", buffer);*/

  list_t *tl = tokenize(buffer);

  assert(tl->size == b->size);
#if 0
  if (tl->size != b->size) {
    for (it = b->first, it2 = tl->first; it && it2;
       it = it->next, it2 = it2->next) {
      fprintf(stderr, "lc1: %d [%s], lc2: %d [%s]\n",
              ((token_t*)it->data)->lcomp,
              ((token_t*)it->data)->lexem,
              ((token_t*)it2->data)->lcomp,
              ((token_t*)it2->data)->lexem
          );
    }
  }
#endif
  for (it = b->first, it2 = tl->first; it && it2;
       it = it->next, it2 = it2->next) {
    assert(!strcmp(((token_t*)it->data)->lexem, ((token_t*)it2->data)->lexem));
    assert(((token_t*)it->data)->lcomp == ((token_t*)it2->data)->lcomp);
  }
  list_destroy(tl);

  list_destroy(b);
}


#define ITERATIONS 100
int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    test_lexer();
  }
  fprintf(stderr, "\n");

  return 0;
}

/* vim: set sw=2 sts=2 : */
