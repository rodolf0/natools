#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>

#include "baas/xstring.h"
#include "parser-priv.h"


parser_t *P = NULL;
long double evaluate(const char *expr) {
  if (!P)
    P = parser_create();
  if (!strcmp(expr, "shutdown")) {
    parser_destroy(P);
    P = NULL;
    return 0.0;
  }
  scanner_t *s = scanner_init(expr);
  long double r = 0.0;
  if (parser_eval(P, s, &r)) {
    fprintf(stderr, "Error evaluating expression\n");
  }
  scanner_destroy(s);
  return r;
}


int main(int argc, char *argv[]) {
  int ret = 0, interactive = 0;

  /* parse command line */
  while ((ret = getopt(argc, argv, "hi")) != -1) {
    switch (ret) {
      case 'h':
        fprintf(stderr, "usage: %s" "<expresion eg: 4/sin(pi^e+2)>\n", argv[0]);
        return 0;
        break;
      case 'i':
        interactive = 1;
        break;
      default:
        return 1;
    }
  }

  if (interactive) {
    char histfile[256], *expr = NULL;

    snprintf(histfile, sizeof(histfile), "%s/.aparser_history", getenv("HOME"));
    using_history();
    read_history(histfile);
    stifle_history(50);

    evaluate("pi");
    while ((expr = readline(">> "))) {
      chomp(expr);
      if (!strcmp(expr, "env")) {
        char **keys;
        size_t j, n = hashtbl_keys(P->symbol_table, &keys);
        for (j = 0; j < n; j++) {
          long double *d = (long double*)hashtbl_get(P->symbol_table, keys[j]);
          printf("%s: %.15Lg\n", keys[j], *d);
          free(keys[j]);
        }
        free(keys);
      } else if (strcmp(expr, ""))
        printf("%.15Lg\n", evaluate(expr));

      add_history(expr);
      free(expr);
    }
    printf("\n");
    write_history(histfile);

  } else if (optind < argc)
    printf("%.15Lg\n", evaluate(argv[optind]));
  else
    fprintf(stderr, "usage: %s" "<expresion eg: 4/sin(pi^e+2)>\n", argv[0]);

  evaluate("shutdown");
  return ret;
}

/* vim: set sw=2 sts=2 : */
