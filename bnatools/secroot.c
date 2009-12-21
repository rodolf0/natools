#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cnash/parser.h>

#define c_epsilon 0.0001
#define c_maxiter 1000

int main(int argc, char *argv[]) {
	double f0, f1;
  double epsilon = c_epsilon;
	double x0 = 0.0, x1;
  int i, max_iter = c_maxiter;
	int ret;


  /* parse command line */
  while ((ret = getopt(argc, argv, "e:i:0:")) != -1) {
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
      default:
				return 1;
    }
  }
	if (optind >= argc) {
		fprintf(stderr, "usage: %s [-e epsilon] [-i max-iterations] -0 <x0> "
                      "<function eg: cos(x)-x^3>\n", argv[0]);
		return 0;
	}

	ret = 0;
	parser_t *p = parser_create();

	/* secant method */
	x1 = x0 + 0.1;
	for (i = 0; i < max_iter; i++) {

		f0 = replace_x();
		f1 = replace_x();

		ret |= parser_eval(p, , &f0);
		ret |= parser_eval(p, , &f1);

		if () {
			fprintf(stderr, "Couldn't eval [%s].\n", argv[optind]);
			break;
		}

		d = 
	}
  
	parser_destroy(p);

  return ret;
}

char *replace_x(const char *buf, double x) {

}

/* vim: set sw=2 sts=2 : */
