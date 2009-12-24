#ifndef _CNASH_H_
#define _CNASH_H_

#define EPSILON "1.0e-15"
#define MAXITER "1000"

typedef struct _interval {
  double x0;
  double x1;
} interval_t;

typedef struct _stop_cond {
  /* error upper bound */
  double epsilon;
  int max_iterations;
} stop_cond_t;

interval_t *interval_create(double x0, double x1);
void interval_destroy(interval_t *i);
void interval_swap(interval_t *i);

stop_cond_t *stop_cond_create(double e, int max_i);
void stop_cond_destroy(stop_cond_t *s);

#endif /* _CNASH_H_ */

/* vim: set sw=2 sts=2 : */
