#include <stdlib.h>
#include <string.h>

#include "parser/scanner.h"

scanner_t *scanner_init(const char *buffer) {

  scanner_t *s = (scanner_t*)malloc(sizeof(scanner_t));

  s->b_length = strlen(buffer);
  s->buffer = (char*)malloc(s->b_length + 1);
  strncpy(s->buffer, buffer, s->b_length);
  s->buffer[s->b_length] = '\0';

  s->pos = -1;
  s->current_char = '\0';
  s->next_char = *s->buffer;

  return s;
}

void scanner_destroy(scanner_t *s) {
  if (s) {
    if (s->buffer)
      free(s->buffer);
    free(s);
  }
}

void scanner_seek(scanner_t *s, size_t pos) {
  s->pos = pos;
  s->current_char = s->buffer[pos];
  s->next_char = s->buffer[pos+1];
}

char scanner_readchar(scanner_t *s) {
  scanner_seek(s, s->pos+1);
  return s->current_char;
}

/* vim: set sw=2 sts=2 : */
