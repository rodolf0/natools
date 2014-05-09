#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>

#include "baas/common.h"
#include "baas/xstring.h"
#include "baas/hashtbl.h"
#include "parser/scanner.h"
#include "parser/lexer.h"
#include "parser/parser.h"

/* Parse a mini-language:
 *   vars        : print defined variables
 *   id '=' expr : assign the result of expr to id (hold it in vars)
 *   expr        : evaluate expr and print result to stdout
 */

hashtbl_t *vars = NULL;

void print_variables(hashtbl_t *v) {
  char **keys;
  size_t j, n = hashtbl_keys(v, &keys);
  for (j = 0; j < n; j++) {
    long double *d = (long double*)hashtbl_get(vars, keys[j]);
    printf("%s: %.15Lg\n", keys[j], *d);
    free(keys[j]);
  }
  free(keys);
}

int parse_asignment(char *var, lexer_t *l) {
  expr_t *e = parser_compile(l);
  if (!e)
    return 1;
  long double *r = zmalloc(sizeof(long double));
  int err = parser_eval(e, r, vars);
  parser_destroy_expr(e);
  if (err)
    free(r);
  else
    hashtbl_insert(vars, var, r);
  return err;
}

int parse_expression(lexer_t *l) {
  expr_t *e = parser_compile(l);
  if (!e)
    return 1;
  long double r = 0.0;
  int err = parser_eval(e, &r, vars);
  parser_destroy_expr(e);
  if (!err)
    printf("%.15Lg\n", r);
  return err;
}

int parse_statement(lexer_t *l) {
  token_t *start = lexer_peek(l);

  if (start->lexcomp == tokId && !strcmp("vars", start->lexem)) {
    lexer_advance(l);
    print_variables(vars);
    return 0;
  }

  if (start->lexcomp == tokId) {
    lexer_advance(l);
    token_t *maybe_assign = lexer_peek(l);
    if (maybe_assign->lexcomp == tokAsign) {
      lexer_advance(l);
      return parse_asignment(start->lexem, l);
    }
    lexer_backup(l);
  }

  return parse_expression(l);
}


int main(int argc, char *argv[]) {
  int ret = 0, interactive = 0;

  /* parse command line */
  while ((ret = getopt(argc, argv, "hi")) != -1) {
    switch (ret) {
      case 'h':
        fprintf(stderr, "usage: aparser {-i | <expresion>}\n");
        return 0;
        break;
      case 'i':
        interactive = 1;
        break;
      default:
        return 1;
    }
  }

  vars = hashtbl_init(free, NULL);

  if (interactive) {
    char histfile[256], *expr = NULL;
    snprintf(histfile, sizeof(histfile), "%s/.aparser_history", getenv("HOME"));
    using_history();
    read_history(histfile);
    stifle_history(50);

    while ((expr = readline(">> "))) {
      if (strcmp(trim(expr), "")) {
        scanner_t *s = scanner_init(expr);
        lexer_t *l = lexer_init(s);
        parse_statement(l);
        lexer_destroy(l);
        scanner_destroy(s);
        add_history(expr);
      }
      free(expr);
    }

    printf("\n");
    write_history(histfile);

  } else while (optind < argc) {
    printf("%.15Lg\n", parser_qeval(argv[optind++]));
  }

  hashtbl_destroy(vars);
  return ret;
}

/* vim: set sw=2 sts=2 : */
