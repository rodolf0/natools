#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <sys/types.h> /* size_t */

#define IS_ALPHA(c) ((c>='a'&& c<='z') || (c>='A'&& c<='Z') ? 1:0)
#define IS_WHITE(c) ((c==' '|| c=='\t' || c=='\n') ? 1:0)
#define IS_NUM(c)   ((c>='0' && c<='9') ? 1:0)
#define MAKES_UNARYOP(c) (c=='+' || c=='-' || c=='*' || \
                          c=='/' || c=='^' || c==',' || \
                          c=='(' || c=='=' || c=='\0' ? 1:0)


typedef struct _scanner_t {
  size_t pos;

  char *buffer;
  size_t b_length;

  char current_char;
  char next_char;

} scanner_t;

char scanner_readchar(scanner_t *s);
void scanner_seek(scanner_t *s, size_t pos);
size_t scanner_get_pos(scanner_t *s);
void scanner_destroy(scanner_t *s);
scanner_t *scanner_init(const char *buffer);

#endif /* _SCANNER_H_ */

/* vim: set sw=2 sts=2 : */
