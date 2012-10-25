#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <sys/types.h> /* size_t */

typedef struct _scanner_t {
  FILE *fp;
  char *buffer;
  size_t cap, size;
  int eof;

  char *current;
  char *next;

  size_t pos;
  char *buffer;
  size_t b_length;
  char current_char;
  char next_char;
} scanner_t;


/* constructor / destructors */
scanner_t * scanner_init_file(const char *file);
scanner_t * scanner_init_fp(FILE *fp);
scanner_t * scanner_init(const char *buffer);
void scanner_destroy(scanner_t *s);

ssize_t scanner_peek(scanner_t *s, size_t n, char *bytes);
ssize_t scanner_read(scanner_t *s, char delim, char *bytes);

ssize_t scanner_readline(scanner_t *s, char *line);
ssize_t scanner_readbytes(scanner_t *s, char delim, char *bytes);

char * scanner_line(scanner_t *s);
char * scanner_word(scanner_t *s);
char * scanner_char(scanner_t *s);

int scanner_seek(scanner_t *s, size_t pos);
char scanner_readchar(scanner_t *s);

#endif /* _SCANNER_H_ */

/* vim: set sw=2 sts=2 : */
