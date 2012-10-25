#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parser/scanner.h"

#define SCANNER_INIT_CAP 128


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
  s->cap = SCANNER_INIT_CAP;
  s->buffer = (char*)malloc(s->cap);
  s->size = fread(s->buffer, sizeof(char), s->cap, s->fp);
  s->eof = feof(s->fp);
  return s;
}


scanner_t *scanner_init(const char *buffer) {
  scanner_t *s = malloc(sizeof(scanner_t));
  memset(s, 0, sizeof(scanner_t));
  s->size = strlen(buffer);
  s->cap = s->size + 1;
  s->buffer = malloc(s->cap);
  strncpy(s->buffer, buffer, s->size);
  s->buffer[s->size] = '\0';
  s->eof = 1;
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


int scanner_seek(scanner_t *s, size_t pos) {
  if (pos > s->b_length)
    return -1;
  s->pos = pos;
  s->current_char = s->buffer[pos];
  s->next_char = (pos == s->b_length ? '\0' : s->buffer[pos+1]);
  return s->pos;
}

char scanner_readchar(scanner_t *s) {
  scanner_seek(s, s->pos+1);
  return s->current_char;
}

/* vim: set sw=2 sts=2 : */
