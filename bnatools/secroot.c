#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <cnash/parser.h>

#define c_epsilon 0.00001
#define c_maxiter 1000

int main(int argc, char *argv[]) {
  double f0, f1 ;
  double x0 = 0.0, x1, d;
  double epsilon = c_epsilon;
  int i, max_iter = c_maxiter;
  int ret, calc_x1 = 1, verbose = 0;
  char buf[256];


  /* parse command line */
  while ((ret = getopt(argc, argv, "ve:i:0:1:")) != -1) {
    switch (ret) {
      case 'e':
        epsilon = atof(optarg);
        break;
      case 'i':
        max_iter = atoi(optarg);
        break;
      case '0':
        x0 = atof(optarg);
        break;
      case '1':
        x1 = atof(optarg);
        calc_x1 = 0;
        break;
      case 'v':
        verbose = 1;
        break;
      default:
        return 1;
    }
  }
  if (optind >= argc || argc < 2) {
    fprintf(stderr, "usage: %s [-v] [-e epsilon] [-i max-iterations] -0 <x0> "
                      "[-1 <x1>] <function eg: cos(x)-x^3>\n", argv[0]);
    return 0;
  }

  ret = 0;
  parser_t *p = parser_create();

  /*                 f(X_n)                   |           f(X_n) - f(X_n-1) 
   * X_n+1 = X_n - --------- (Newton's method)|f'(X_n) ~ -------------------
   *                f'(X_n)                   |              X_n - X_n-1
   *
   *                   X_n - X_n-1
   * X_n+1 = X_n - ------------------- * f(X_n)
   *                f(X_n) - f(X_n-1)
   * */

  /* secant method */
  x1 = calc_x1 ? x0 + 1.0 : x1;
  for (i = 0; i < max_iter; i++) {

    /* load x=x0 into the parser */
    snprintf(buf, sizeof(buf), "x=%f", x0);
    ret |= parser_eval(p, buf, &f0);
    /* evaluate f(x0) */
    ret |= parser_eval(p, argv[optind], &f0);

    /* load x=x1 into the parser */
    snprintf(buf, sizeof(buf), "x=%f", x1);
    ret |= parser_eval(p, buf, &f1);
    /* evaluate f(x1) */
    ret |= parser_eval(p, argv[optind], &f1);

    if (ret) {
      fprintf(stderr, "Couldn't eval [%s] x0=%f x1=%f.\n",
              argv[optind], x0, x1);
      break;
    }

    d = f1 * (x1 - x0) / (f1 - f0);
    if (fabs(d) < epsilon)
      break;

    x0 = x1;
    x1 = x1 - d;

    if (verbose) {
      fprintf(stderr, "x=%f; f(x)=%f; d=%f; xn-1=%f\n",
                      x1, f1, d, x0);
    }
  }

  printf("%.15g\n", x1);
  
  parser_destroy(p);

  return ret;
}

/* vim: set sw=2 sts=2 : */
