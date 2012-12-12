#ifndef _CALCULUS_H_
#define _CALCULUS_H_

#include "natools/function.h"

/* 1st, 2nd, 3rd order derivates */
long double derivate_1(function_t *f, long double x0);
long double derivate_2(function_t *f, long double x0);
long double derivate_3(function_t *f, long double x0);

/* calculate nth derivate using finite differences */
long double derivate(function_t *f, int n, long double x0);

/* integrate f between x0 and x1 using simpson */
long double integrate_simpson(function_t *f, long double x0, long double x1);

#endif /* _CALCULUS_H_  */

/* vim: set sw=2 sts=2 : */
