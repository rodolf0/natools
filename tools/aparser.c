#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>

#include "baas/hashtbl.h"
#include "parser/scanner.h"
#include "parser/lexer.h"
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
      scanner_t *s = scanner_init(expr);
      lexer_t *l = lexer_init(s);

      token_t *first = lexer_advance(l);

      if (!strcmp(first->lexem, "vars")) {
        lexer_consume(l);

        char **keys;
        size_t j, n = hashtbl_keys(vars, &keys);
        for (j = 0; j < n; j++) {
          long double *d = (long double*)hashtbl_get(vars, keys[j]);
          printf("%s: %.20Lg\n", keys[j], *d);
          free(keys[j]);
        }
        free(keys);

      } else {
        token_t *maybe_assign = lexer_advance(l);
        /* identify an assignment */
        if (first->lexcomp == tokId && maybe_assign->lexcomp == tokAsign) {
          expr_t *e = parser_compile(l);
          long double *r = malloc(sizeof(long double));
          if (!parser_eval(e, r, vars)) {
            hashtbl_insert(vars, first->lexem, r);
          } else {
            free(r);
          }
          parser_destroy_expr(e);
        } else {
          lexer_backup(l); /* maybe = */
          lexer_backup(l); /* first */
          long double r = 0.0;
          expr_t *e = parser_compile(l);
          parser_eval(e, &r, vars);
          printf("%.20Lg\n", r);
          parser_destroy_expr(e);
        }
      }

      lexer_destroy(l);
      scanner_destroy(s);
      add_history(expr);
      free(expr);
    }

    printf("\n");
    write_history(histfile);

  } else if (optind < argc) {
    printf("%.20Lg\n", parser_qeval(argv[optind]));
  } else {
    help();
  }

  hashtbl_destroy(vars);
  return ret;
}

/* vim: set sw=2 sts=2 : */
