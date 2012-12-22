#ifndef _PARSER_H_
#define _PARSER_H_

typedef struct _scanner_t scanner_t;
typedef struct _hashtbl_t hashtbl_t;
typedef struct _expr_t expr_t;

expr_t * parser_compile(scanner_t *s);
void parser_destroy_expr(expr_t *e);

int parser_eval(const expr_t *e, long double *r, hashtbl_t *vars);

#endif /* _PARSER_H_ */

/* vim: set sw=2 sts=2 : */
