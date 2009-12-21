#include <stdio.h>
#include <cnash/parser.h>

int main(int argc, const char *argv[]) {
	int ret = 0;
	double r = 0.0;

	if (argc <= 1) {
		fprintf(stderr, "usage: %s <expresion>\n", argv[0]);
		return 0;
	}

  parser_t *p = parser_create();

	if ((ret = parser_eval(p, argv[1], &r)))
		fprintf(stderr, "Couldn't evaluate [%s].\n", argv[1]);
	else
		printf("%.15g\n", r);

	parser_destroy(p);

  return ret;
}

/* vim: set sw=2 sts=2 : */
