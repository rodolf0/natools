#ifndef _XSTRING_H
#define _XSTRING_H_

size_t strcount(const char *where, const char *what);

/* dest must be a buffer large enough to hold the result
 * return: number of replacements. */
size_t strrep(char *dest, const char *source,
							const char *what, const char *with);

void chomp(char *s);

#endif /* _XSTRING_H_ */

/* vim: set sw=2 sts=2 : */
