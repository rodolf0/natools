/* Extended string functions */

#include <string.h>

/* return occurrences of what in where */
size_t strcount(const char *where, const char *what) {
  size_t count = 0;
  const char *offset = where;

	if (where && what && what[0]) {
		while ((offset = strstr(offset, what))) {
			count++;
			offset++;
		}
	}
  return count;
}

/* returns number of replacements */
size_t strrep(char *dest, const char *source,
							const char *what, const char *with) {
	if (!dest || !source || !what || !with)
		return 0;
	if (!what[0]) /* something to replace */
		return 0;

	size_t len_what = strlen(what);
  size_t len_with = strlen(with);
	const char *off_source, *off_found;
	char *off_dest;
	size_t n, count = 0;

	off_dest = dest;
	off_source = source;
	off_found = source;
			
	while ((off_found = strstr(off_found, what))) {
		/* copy prefix */
		strncpy(off_dest, off_source, off_found - off_source);
		off_dest += off_found - off_source;
		/* copy replacement */
		strncpy(off_dest, with, len_with);
		off_dest += len_with;

		/* advance start copy  & search pointers */
		off_source = off_found + len_what;
		off_found += len_what;

		count++;
	}
	
	/* copy suffix */
	n = strlen(off_source);
	strncpy(off_dest, off_source, n);
	off_dest[n] = '\0';
	
	return count;
}

/* vim: set sw=2 sts=2 : */
