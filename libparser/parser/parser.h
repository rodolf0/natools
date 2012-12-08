#ifndef _PARSER_H_
#define _PARSER_H_

typedef struct _parser_t parser_t;
typedef struct _scanner_t scanner_t;

parser_t *parser_create();
void parser_destroy(parser_t *p);

int parser_setvar(parser_t *p, char *var, long double val);
int parser_eval(parser_t *p, scanner_t *s, long double *result);

#endif /* _PARSER_H_ */

/* vim: set sw=2 sts=2 : */
