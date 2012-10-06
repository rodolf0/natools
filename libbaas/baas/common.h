#ifndef _BAAS_COMMON_H_
#define _BAAS_COMMON_H_

#include <sys/types.h> /* size_t */

typedef int (*cmp_func_t)(const void *, const void *);
typedef void (*free_func_t)(void *);
typedef size_t (*hash_func_t)(const char *);

#endif /* _BAAS_COMMON_H_ */

/* vim: set sw=2 sts=2 : */
