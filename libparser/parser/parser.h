#ifndef _PARSER_H_
#define _PARSER_H_

typedef struct _lexer_t lexer_t;
typedef struct _hashtbl_t hashtbl_t;
typedef struct _expr_t expr_t;

/* compile an expression from a string for later evaluation */
expr_t * parser_compile(lexer_t *s);
expr_t * parser_compile_str(const char *str);
/* destructor for compiled expressions */
void parser_destroy_expr(expr_t *e);

/* evaluate a compiled expression using variables from vars */
int parser_eval(const expr_t *e, long double *r, hashtbl_t *vars);
/* quick-evaluate an expression, only internal constants are available */
long double parser_qeval(const char *expr);

#endif /* _PARSER_H_ */

/* vim: set sw=2 sts=2 : */
