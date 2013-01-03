#ifndef _FUNCTION_H_
#define _FUNCTION_H_

typedef struct function_t function_t;

function_t * function_create(const char *func);
void function_destroy(function_t *f);
long double function_eval(function_t *f, long double x0);

#endif /* _FUNCTION_H_ */

/* vim: set sw=2 sts=2 : */
