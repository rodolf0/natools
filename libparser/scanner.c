#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parser/scanner.h"

#define SCANNER_BUF_SZ 1024

typedef struct _scanner_t {
  FILE *fp;
  char *buffer;
  size_t buf_sz;
  size_t buf_cap;
  /* boundaries of current scanned item */
  size_t start;
  size_t length;
} scanner_t;


scanner_t * scanner_init_file(const char *file) {
  if (strcmp(file, "-") == 0)
    return scanner_init_fp(stdin);
  return scanner_init_fp(fopen(file, "rb"));
}

scanner_t * scanner_init_fp(FILE *fp) {
  if (!fp)
    return NULL;
  scanner_t *s = (scanner_t*)malloc(sizeof(scanner_t));
  memset(s, 0, sizeof(scanner_t));
  s->fp = fp;
  s->length = s->start = 0;
  s->buf_cap = SCANNER_BUF_SZ;
  s->buffer = (char*)malloc(s->buf_cap);
  /* initial buffer fill */
  int r = fread(s->buffer, 1, s->buf_cap, s->fp);
  if (r < 0) {
    free(s->buffer); free(s);
    return NULL;
  }
  s->buf_sz = r;
  return s;
}

scanner_t *scanner_init(const char *buffer) {
  scanner_t *s = malloc(sizeof(scanner_t));
  memset(s, 0, sizeof(scanner_t));
  s->length = s->start = 0;
  s->buf_cap = s->buf_sz = strlen(buffer);
  s->buffer = (char*)malloc(s->buf_cap + 1);
  /* initial buffer fill */
  memmove(s->buffer, buffer, s->buf_sz);
  s->buffer[s->buf_cap] = 0;
  return s;
}

void scanner_destroy(scanner_t *s) {
  if (!s)
    return;
  if (s->fp && s->fp != stdin)
    fclose(s->fp);
  if (s->buffer)
    free(s->buffer);
  free(s);
}


/* shift out previous input and refill buffer (return num read -1: error) */
static int scanner_shift_n_fill(scanner_t *s) {
  if (!s || s->start == 0)
    return 0;
  /* shift available buffer to the beggining */
  memmove(s->buffer, s->buffer + s->start, s->buf_sz - s->start);
  s->buf_sz -= s->start;
  s->start = 0;
  /* read in more data */
  int r = 0;
  if (s->fp && !feof(s->fp)) {
    r = fread(s->buffer + s->buf_sz, sizeof(char),
              s->buf_cap - s->buf_sz, s->fp);
    if (r > 0)
      s->buf_sz += r;
  }
  return r;
}


char scanner_advance(scanner_t *s) {
  if (!s)
    return 0;
  if (s->start + s->length <= s->buf_sz)
    s->length++;
  if (s->start + s->length > s->buf_sz)
    scanner_shift_n_fill(s);
  return scanner_current(s);
}

char scanner_peek(scanner_t *s) {
  size_t l = s->length;
  char p = scanner_advance(s);
  s->length = l;
  return p;
}

char scanner_current(scanner_t *s) {
  if (!s || s->length == 0 || s->start + s->length > s->buf_sz)
    return 0;
  return s->buffer[s->start + s->length - 1];
}

char scanner_backup(scanner_t *s) {
  if (!s || s->length <= 0)
    return 0;
  s->length--;
  return scanner_current(s);
}


void * scanner_accept(scanner_t *s, acceptfn f) {
  if (!s || s->length == 0)
    return NULL;
  void *r = NULL;
  if (f)
    r = f(s->buffer + s->start, s->length);
  s->start += s->length;
  s->length = 0;
  return r;
}

void scanner_ignore(scanner_t *s) {
  scanner_accept(s, NULL);
}

void * scanner_apply(scanner_t *s, acceptfn f) {
  if (!s || s->length == 0)
    return NULL;
  void *r = NULL;
  if (f)
    r = f(s->buffer + s->start, s->length);
  return r;
}

/* vim: set sw=2 sts=2 : */
