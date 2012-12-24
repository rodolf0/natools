#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>

#include "baas/xstring.h"
#include "baas/hashtbl.h"
#include "parser/parser.h"

void help() {
  fprintf(stderr, "usage: aparser {-i | <expresion>}\n");
}

int main(int argc, char *argv[]) {
  int ret = 0, interactive = 0;

  /* parse command line */
  while ((ret = getopt(argc, argv, "hi")) != -1) {
    switch (ret) {
      case 'h':
        help();
        return 0;
        break;
      case 'i':
        interactive = 1;
        break;
      default:
        return 1;
    }
  }

  hashtbl_t *vars = hashtbl_init(free, NULL, 0);
  if (interactive) {
    char histfile[256], *expr = NULL;

    snprintf(histfile, sizeof(histfile), "%s/.aparser_history", getenv("HOME"));
    using_history();
    read_history(histfile);
    stifle_history(50);

    while ((expr = readline(">> "))) {
      chomp(expr);

      if (!strcmp(expr, "vars")) {
        char **keys;
        size_t j, n = hashtbl_keys(vars, &keys);
        for (j = 0; j < n; j++) {
          long double *d = (long double*)hashtbl_get(vars, keys[j]);
          printf("%s: %.15Lg\n", keys[j], *d);
          free(keys[j]);
        }
        free(keys);

      } else if (strcmp(expr, "")) {
        expr_t *e = parser_compile_str(expr);
        long double d = 0.0;
        parser_eval(e, &d, vars);
        printf("%.25Lg\n", d);
        parser_destroy_expr(e);
      }

      add_history(expr);
      free(expr);
    }
    printf("\n");
    write_history(histfile);

  } else if (optind < argc)
    printf("%.25Lg\n", parser_qeval(argv[optind]));
  else
    help();

  hashtbl_destroy(vars);
  return ret;
}

/* vim: set sw=2 sts=2 : */
