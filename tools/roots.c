#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "natools/natools.h"
#include "natools/function.h"
#include "natools/equation_solver.h"

int main(int argc, char *argv[]) {
  int ret;
  char *x0 = NULL, *x1 = NULL;
  char *epsilon = EPSILON, *func = NULL;
  char *max_iter = MAXITER, *method = "secant";
  long double r;

  /* parse command line */
  while ((ret = getopt(argc, argv, "vm:e:i:a:b:")) != -1) {
    switch (ret) {
      case 'e':
        epsilon = optarg; /* optarg is just a pointer to argv */
        break;
      case 'i':
        max_iter = optarg;
        break;
      case 'a':
        x0 = optarg;
        break;
      case 'b':
        x1 = optarg;
        break;
      case 'm':
        method = optarg;
        break;
      case 'v':
        root_search_verbose = 1;
        break;
      default:
        return 1;
    }
  }
  if (optind >= argc || !x0) {
    fprintf(stderr, "usage: %s [-v] [-e <epsilon>] [-i <max-iter>] [-m <method>]"
            "-a <x0> [-b <x1>] <function eg: cos(x)-x^3>\n", argv[0]);
    return 1;
  } else {
    func = argv[optind];
  }

  function_t *f = function_create(func);
  interval_t *i = interval_create(atof(x0), x1 ? atof(x1) : (atof(x0) + 1.0));
  stop_cond_t *s = stop_cond_create(atof(epsilon), atoi(max_iter));

  if (!strcmp(method, "secant"))
    ret = root_secant(f, i, s, &r);
  else if (!strcmp(method, "bisection"))
    ret = root_bisection(f, i, s, &r);
  else if (!strcmp(method, "regulafalsi"))
    ret = root_regulafalsi(f, i, s, &r);
  else if (!strcmp(method, "newton"))
    ret = root_newton(f, atof(x0), s, &r);
  else {
    fprintf(stderr, "Method not recognized: %s\n", method);
    return 1;
  }

  if (ret) {
    fprintf(stderr, "%s method failed (ret: %d)\n", method, ret);
  } else {
    printf("%.15Lg\n", r);
  }

  function_destroy(f);
  interval_destroy(i);
  stop_cond_destroy(s);

  return ret;
}

/* vim: set sw=2 sts=2 : */
