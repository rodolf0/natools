#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <sys/types.h> /* size_t */

typedef struct _scanner_t {
  FILE *fp;
  char *buffer;
  size_t buf_sz;
  size_t buf_cap;
  /* boundaries of current scanned item */
  size_t start;
  size_t length;
} scanner_t;

/* action type fn to invoke when accepting a char string */
typedef void * (*acceptfn)(char *start, size_t len);

/* constructor / destructors */
scanner_t * scanner_init_file(const char *file);
scanner_t * scanner_init_fp(FILE *fp);
scanner_t * scanner_init(const char *buffer);
void scanner_destroy(scanner_t *s);

/* get the next char in the input stream (0 means eof or token too large) */
char scanner_advance(scanner_t *s);
/* get the next char without advancing (0 means eof or token too large) */
char scanner_peek(scanner_t *s);
/* backup a char and get the next (0 means we're at the start pos again) */
char scanner_backup(scanner_t *s);
/* accept current slice and start a new one (return result of f) */
void * scanner_accept(scanner_t *s, acceptfn f);
/* ignore current slice and start a new one */
void scanner_ignore(scanner_t *s);

#endif /* _SCANNER_H_ */

/* vim: set sw=2 sts=2 : */
