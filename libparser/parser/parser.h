#ifndef _PARSER_H_
#define _PARSER_H_

#include <baas/list.h>

typedef struct _parser {
  /* list of token_t's. in this case only keep variables */
  list_t *symbol_table;
  list_t *parser_stack;
  list_t *result_stack;
  /* input buffer */
  list_t *tokenlist;

  const char (*operator_precedence)[14][14];
} parser_t;


parser_t *parser_create();
void parser_destroy(parser_t *p);
int parser_eval(parser_t *p, const char* buf, double *ret);

#endif /*  _PARSER_H_ */

/* vim: set sw=2 sts=2 : */
