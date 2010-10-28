#ifndef _BIGNUM_H_
#define _BIGNUM_H_

/* results returned should be freed since they're dynamically alloc'd */

/* adds to numbers with no presicion limit */
char * big_add(const char *x, const char *y);

/* multiplies to numbers with no presicion limit */
char * big_mul(const char *x, const char *y);

#endif

/* vim: set sw=2 sts=2 : */
