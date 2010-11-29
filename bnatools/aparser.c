#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <unistd.h>
#include <parser/lexer.h>
#include <parser/parser.h>
#include <baas/list.h>
#include <baas/xstring.h>

int main(int argc, char *argv[]) {
  int ret = 0, verbose = 0, interactive = 0;
  double r = 0.0;
  list_node_t *n;

  /* parse command line */
  while ((ret = getopt(argc, argv, "vhi")) != -1) {
    switch (ret) {
      case 'v':
        verbose = 1;
        break;
      case 'h':
        fprintf(stderr, "usage: %s [-v (verbose)] "
                        "<expresion eg: 4/sin(pi^e+2)>\n", argv[0]);
        return 0;
        break;
      case 'i':
        interactive = 1;
        break;
      default:
        return 1;
    }
  }

  parser_t *p = parser_create();

  if (interactive) {
    char *buf = NULL;
    while ((buf = readline(">> "))) {
      chomp(buf);

      if ((ret = parser_eval(p, buf, &r)))
        fprintf(stderr, "Couldn't evaluate [%s].\n", buf);
      else
        printf("%.15g\n", r);

      if (verbose) {
        for (n = p->symbol_table->first; n; n = n->next)
          fprintf(stderr, "%s = %f\n",
              ((token_t*)n->data)->lexem,
              ((token_t*)n->data)->value);
      }
      free(buf);
    }

  } else if (optind < argc) {
    if ((ret = parser_eval(p, argv[optind], &r)))
      fprintf(stderr, "Couldn't evaluate [%s].\n", argv[optind]);
    else
      printf("%.15g\n", r);
  }

  parser_destroy(p);

  return ret;
}

/* vim: set sw=2 sts=2 : */
