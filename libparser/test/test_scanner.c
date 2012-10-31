#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../parser/scanner.h"

char * test_accept_fn(char *start, size_t len) {
  char *r = malloc(sizeof(char) * len + 1);
  memcpy(r, start, len);
  r[len] = '\0';
  return r;
}

void test_buffer_walk() {
  char *b = "this is a small test buffer";
  scanner_t *s = scanner_init(b);
  assert(strlen(s->buffer) == strlen(b));

  assert(scanner_peek(s) == 't');
  assert(scanner_advance(s) == 't');
  assert(scanner_advance(s) == 'h');
  assert(scanner_advance(s) == 'i');
  assert(scanner_advance(s) == 's');
  assert(scanner_peek(s) == ' ');
  assert(scanner_current(s) == 's');
  assert(scanner_advance(s) == ' ');
  assert(scanner_backup(s) == 's');

  char *this = (char*)scanner_accept(s, (acceptfn)test_accept_fn);
  assert(strcmp(this, "this") == 0);
  free(this);

  size_t i;
  char c;
  for (i = s->start + s->length; i < s->buf_sz; i++) {
    c = scanner_advance(s);
    assert(c == b[i]);
    /*fprintf(stderr, "%c", c);*/
  }

  scanner_destroy(s);
}

void test_file_walk(const char *file) {
  scanner_t *s = scanner_init_file(file);
  char *start = "#include <assert.h>";
  size_t i, l;
  char c;

  l = strlen(start);
  for (i = 0; i < l; i++) {
    c = scanner_advance(s);
    assert(c == start[i]);
    /*fprintf(stderr, "%c", c);*/
  }

  char *tok = (char*)scanner_accept(s, (acceptfn)test_accept_fn);
  assert(strcmp(tok, start) == 0);
  free(tok);

  scanner_destroy(s);
}

int main(int argc, char *argv[]) {
  fprintf(stderr, "Testing... ");
  test_buffer_walk();
  /*fprintf(stderr, "\nTesting file... ");*/
  test_file_walk("test_scanner.c");
  fprintf(stderr, "done\n");
  return 0;
}

/* vim: set sw=2 sts=2 : */
