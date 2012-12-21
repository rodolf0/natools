#ifndef _PARSER_H_
#define _PARSER_H_

typedef struct _parser_t parser_t;
typedef struct _scanner_t scanner_t;
typedef struct _hashtbl_t hashtbl_t;
typedef struct _expr_t expr_t;

parser_t *parser_create();
void parser_destroy(parser_t *p);

expr_t * parser_compile(parser_t *p, scanner_t *s);
void parser_destroy_expr(expr_t *e);

int evaluate_expression(const expr_t *e, long double *r,
                        hashtbl_t *vars, hashtbl_t *funcs);

#endif /* _PARSER_H_ */

/* vim: set sw=2 sts=2 : */
