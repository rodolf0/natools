#ifndef _CNASH_H_
#define _CNASH_H_

#include <sys/types.h>

#define EPSILON "1.0e-15"
#define MAXITER "1000"

typedef struct _interval {
  long double x0;
  long double x1;
} interval_t;

interval_t *interval_create(long double x0, long double x1);
void interval_destroy(interval_t *i);
void interval_swap(interval_t *i);


typedef struct _stop_cond {
  /* error upper bound */
  long double epsilon;
  size_t max_iterations;
} stop_cond_t;

stop_cond_t *stop_cond_create(long double e, int max_i);
void stop_cond_destroy(stop_cond_t *s);


typedef struct _vector2 {
  long double x0;
  long double x1;
} vector2_t;

vector2_t *vector2_create(long double x0, long double x1);
void vector2_destroy(vector2_t *v);


typedef struct _matrix {
  long double *a;
  size_t r, c;
} matrix_t;

matrix_t *matrix_create(size_t r, size_t c);
void matrix_destroy(matrix_t *m);
long double *matrix_at(matrix_t *m, size_t r, size_t c);

typedef struct _vector {
  long double *a;
  size_t n;
} vector_t;

vector_t *vector_create(size_t d);
void vector_destroy(vector_t *v);
long double *vector_at(vector_t *v, size_t n);

#endif /* _CNASH_H_ */

/* vim: set sw=2 sts=2 : */
