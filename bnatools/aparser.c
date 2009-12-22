#include <stdio.h>
#include <unistd.h>
#include <cnash/parser.h>
#include <cnash/lexer.h>
#include <cnash/list.h>

int main(int argc, char *argv[]) {
  int ret = 0;
  int verbose = 0;
  double r = 0.0;
  list_node_t *n;

  /* parse command line */
  while ((ret = getopt(argc, argv, "v")) != -1) {
    switch (ret) {
      case 'v':
        verbose = 1;
        break;
      default:
        return 1;
    }
  }
  if (optind >= argc) {
    fprintf(stderr, "usage: %s [-v (verbose)] <expresion eg: 4/sin(pi^e+2)>\n", argv[0]);
    return 0;
  }

  parser_t *p = parser_create();

  if ((ret = parser_eval(p, argv[optind], &r)))
    fprintf(stderr, "Couldn't evaluate [%s].\n", argv[optind]);
  else
    printf("%.15g\n", r);

  if (verbose) {
    for (n = p->symbol_table->first; n; n = n->next)
      fprintf(stderr, "%s = %f\n", 
              ((token_t*)n->data)->lexem,
              ((token_t*)n->data)->value);
  }

  parser_destroy(p);

  return ret;
}

/* vim: set sw=2 sts=2 : */
