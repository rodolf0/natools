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

  size_t i = 4;
  char c;
  while ((c = scanner_advance(s)))
    assert(c == b[i++]);
  assert(strlen(b) == i);

  scanner_destroy(s);
}

void test_file_walk(const char *file) {
  char buf[4000];
  FILE *f = fopen(file, "rb");
  int i, j, n = fread(buf, sizeof(char), sizeof(buf), f);
  fclose(f);

  scanner_t *s = scanner_init_file(file);

  for (j = 0; j < n; j+=16) {
    for (i = 0; i < 16 && i+j < n; i++)
      assert(scanner_advance(s) == buf[j+i]);
    char *tok = (char*)scanner_accept(s, (acceptfn)test_accept_fn);
    assert(strncmp(tok, buf+j, 16) == 0);
    free(tok);
  }

  scanner_destroy(s);
}


void test_eof_bof() {
  char *b = "test";
  scanner_t *s = scanner_init(b);

  assert(scanner_peek(s) == 't');
  assert(scanner_advance(s) == 't');
  assert(scanner_advance(s) == 'e');
  assert(scanner_peek(s) == 's');
  assert(scanner_advance(s) == 's');
  assert(scanner_advance(s) == 't');
  assert(scanner_peek(s) == 0);
  assert(scanner_current(s) == 't');
  assert(scanner_advance(s) == 0);
  assert(scanner_advance(s) == 0);
  assert(scanner_advance(s) == 0);
  assert(scanner_advance(s) == 0);
  assert(scanner_current(s) == 0);
  assert(scanner_backup(s) == 't');
  assert(scanner_backup(s) == 's');
  assert(scanner_peek(s) == 't');
  assert(scanner_backup(s) == 'e');
  assert(scanner_backup(s) == 't');
  assert(scanner_backup(s) == 0);
  assert(scanner_current(s) == 0);
  assert(scanner_peek(s) == 't');

  scanner_destroy(s);
}

void test_eof_bof_after_accept() {
  char *b = "test this";
  scanner_t *s = scanner_init(b);

  assert(scanner_advance(s) == 't');
  assert(scanner_advance(s) == 'e');
  assert(scanner_advance(s) == 's');
  assert(scanner_advance(s) == 't');
  assert(scanner_advance(s) == ' ');
  scanner_ignore(s);
  assert(scanner_current(s) == 0);
  assert(scanner_advance(s) == 't');
  assert(scanner_advance(s) == 'h');
  assert(scanner_advance(s) == 'i');
  assert(scanner_advance(s) == 's');
  assert(scanner_advance(s) == 0);
  assert(scanner_advance(s) == 0);
  assert(scanner_current(s) == 0);
  assert(scanner_backup(s) == 's');
  assert(scanner_backup(s) == 'i');
  assert(scanner_backup(s) == 'h');
  assert(scanner_backup(s) == 't');
  assert(scanner_backup(s) == 0);

  scanner_destroy(s);
}

int main(int argc, char *argv[]) {
  fprintf(stderr, "Testing... ");
  test_buffer_walk();
  test_file_walk("test_scanner.c");
  test_eof_bof();
  test_eof_bof_after_accept();
  fprintf(stderr, "done\n");
  return 0;
}

/* vim: set sw=2 sts=2 : */
