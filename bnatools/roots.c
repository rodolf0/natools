#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <cnash/root_search.h>

#define c_epsilon "1.0e-15"
#define c_maxiter "1000"

int main(int argc, char *argv[]) {
  int ret, verbose = 0;
  char *x0 = NULL, *x1 = NULL;
  char *epsilon = c_epsilon, *func = NULL;
  char *max_iter = c_maxiter, *method = "secant";

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
        verbose = 1;
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

  roots_t *rs = roots_init(func, atof(x0), x1 ? atof(x1) : (atof(x0) + 1.0), 
                           atof(epsilon), atoi(max_iter));

  rs->verbose = verbose;

  if (!strcmp(method, "secant"))
    ret = root_secant(rs);
  else if (!strcmp(method, "bisection"))
    ret = root_bisection(rs);
  else if (!strcmp(method, "regulafalsi"))
    ret = root_regulafalsi(rs);
  else if (!strcmp(method, "newton"))
    ret = root_newton(rs);
  else {
    fprintf(stderr, "Method not recognized: %s\n", method);
    return 1;
  }

  if (ret) {
    fprintf(stderr, "%s method failed (ret: %d)\n", method, ret);
  } else {
    printf("%.15g\n", rs->root);
  }

  roots_destroy(rs);
  
  return ret;
}

/* vim: set sw=2 sts=2 : */
