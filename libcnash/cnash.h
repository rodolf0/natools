#ifndef _CNASH_H_
#define _CNASH_H_

#define EPSILON "1.0e-15"
#define MAXITER "1000"

typedef struct _interval {
  double x0;
  double x1;
} interval_t;

interval_t *interval_create(double x0, double x1);
void interval_destroy(interval_t *i);
void interval_swap(interval_t *i);


typedef struct _stop_cond {
  /* error upper bound */
  double epsilon;
  int max_iterations;
} stop_cond_t;

stop_cond_t *stop_cond_create(double e, int max_i);
void stop_cond_destroy(stop_cond_t *s);


typedef struct _vector2 {
  double x0;
  double x1;
} vector2_t;

vector2_t *vector2_create(double x0, double x1);
void vector2_destroy(vector2_t *v);

#endif /* _CNASH_H_ */

/* vim: set sw=2 sts=2 : */
