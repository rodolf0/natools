#include <stdlib.h>
#include <string.h>

#include "parser/scanner.h"

scanner_t *scanner_init(const char *buffer) {
  scanner_t *s = malloc(sizeof(scanner_t));

  s->b_length = strlen(buffer);
  s->buffer = malloc(s->b_length + 1);
  strncpy(s->buffer, buffer, s->b_length);
  s->buffer[s->b_length] = '\0';

  s->pos = -1;
  s->current_char = '\0';
  s->next_char = *s->buffer;

  return s;
}

void scanner_destroy(scanner_t *s) {
  if (!s)
    return;
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
