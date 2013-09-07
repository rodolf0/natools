#ifndef _XSTRING_H_
#define _XSTRING_H_

#include "common.h"

size_t strcount(const char *where, const char *what);

/* dest must be a buffer large enough to hold the result
 * return: number of replacements. */
size_t strrep(char *dest, const char *source,
              const char *what, const char *with);

/* remove */
void chomp(char *s);

/* remove spaces surrounding 's' (changes original string) */
char * trim(char *s);

#endif /* _XSTRING_H_ */

/* vim: set sw=2 sts=2 : */
