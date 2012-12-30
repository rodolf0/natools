#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <stdio.h>

typedef struct _scanner_t scanner_t;

/* action type fn to invoke when accepting a char string */
typedef void * (*acceptfn)(char *start, size_t len);

/* constructor / destructors */
scanner_t * scanner_init_file(const char *file);
scanner_t * scanner_init_fp(FILE *fp);
scanner_t * scanner_init(const char *buffer);
void scanner_destroy(scanner_t *s);

/* get the next input char (0 means eof or token too large) */
char scanner_advance(scanner_t *s);
/* show the next char without consuming it (0 means eof or token too large) */
char scanner_peek(scanner_t *s);
/* show the current char (0 may mean eof or bof) */
char scanner_current(scanner_t *s);
/* get the prev char (0 means we're at the start pos again) */
char scanner_backup(scanner_t *s);
/* accept current slice and start a new one (return result of f) */
void * scanner_accept(scanner_t *s, acceptfn f);
/* ignore current slice and start a new one */
void scanner_ignore(scanner_t *s);
/* apply the given function on the current slice without accepting */
void * scanner_apply(scanner_t *s, acceptfn f);

#endif /* _SCANNER_H_ */

/* vim: set sw=2 sts=2 : */
