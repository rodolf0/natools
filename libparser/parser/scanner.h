#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <sys/types.h> /* size_t */

typedef struct _scanner_t {
  size_t pos;
  char *buffer;
  size_t b_length;
  char current_char;
  char next_char;
} scanner_t;

/* constructor / destructors */
scanner_t *scanner_init(const char *buffer);
void scanner_destroy(scanner_t *s);

int scanner_seek(scanner_t *s, size_t pos);
char scanner_readchar(scanner_t *s);

#endif /* _SCANNER_H_ */

/* vim: set sw=2 sts=2 : */
