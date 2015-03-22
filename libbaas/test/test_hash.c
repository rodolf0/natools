#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "baas/hashtbl.h"
#include "baas/vector.h"

/******** Some aux functions for testing ***********/
int intcmp(const int *a, const int *b) {
  if (!a && !b)
    return 0;
  if (!a && b)
    return 1;
  if (a && !b)
    return -1;
  return *a > *b ? 1 : (*a < *b ? -1 : 0);
}

int * _rint(void) {
  int *e = (int*)zmalloc(sizeof(int));
  *e = random() % 54321;
  return e;
}

char * _intkey(int i) {
  char *k = (char*)zmalloc(32);
  sprintf(k, "%d", i);
  return k;
}

int _acum_sum = 0;
int _acum_count = 0;
void acumulate(int *x) {
  _acum_sum += *x;
  _acum_count++;
}

/**************************************************/
void check_keys(hashtbl_t *h, vector_t **keys) {
  char **keyz;
  size_t nkeys = hashtbl_keys(h, &keyz);
  assert(nkeys == hashtbl_size(h));
  for (size_t j = 0; j < nkeys; ++j) {
    ssize_t found = vector_find(*keys, keyz[j]);
    assert(found >= 0);
    *keys = vector_remove(*keys, found);
    int *e = (int*)hashtbl_get(h, keyz[j]);
    assert(atoi(keyz[j]) == *e);
    free(keyz[j]);
  }
  free(keyz);
  assert(vector_size(*keys) == 0);
}

/**************************************************/
hashtbl_t * generate_test_ht(size_t size, int allow_dups) {
  hashtbl_t *h = hashtbl_init(free, (cmp_func_t)intcmp);
  hashtbl_allow_dups(h, allow_dups);
  vector_t *keys = vector_init(free, (cmp_func_t)strcmp);
  size_t count = 0;
  int sum = 0;

  while (hashtbl_size(h) < size) {
    /* flip a coin to exersice different insertion ops */
    int choice = random() % 100;
    if (choice < 70) {
      int *e = _rint();
      char *k = _intkey(*e);
      if (vector_find(keys, k) < 0 || allow_dups != 0) {
        count++; sum += *e;
        keys = vector_append(keys, k);
      }
      assert(hashtbl_insert(h, k, e) != NULL);
      assert(hashtbl_size(h) == count);
    } else if (hashtbl_size(h) > 0 && choice < 85) {
      size_t idx = random() % vector_size(keys);
      char *k = (char*)vector_get(keys, idx);
      if (allow_dups == 0) {
        count--; sum -= *(int*)hashtbl_get(h, k);
        assert(hashtbl_delete(h, k) == 1);
        keys = vector_remove(keys, idx);
      } else {
        char kk[33]; strcpy(kk, k);
        size_t delcount = 0;
        ssize_t f = vector_find(keys, kk);
        while (f >= 0) {
          count--; sum -= atoi((char*)vector_get(keys, f));
          delcount++;
          keys = vector_remove(keys, f);
          f = vector_find(keys, kk);
        }
        size_t oldsz = hashtbl_size(h);
        assert(hashtbl_delete(h, kk) == delcount);
        assert(hashtbl_size(h) == oldsz - delcount);
      }
    } else if (hashtbl_size(h) > 0 && choice < 100) {
      size_t idx = random() % vector_size(keys);
      char *k = (char*)vector_get(keys, idx);
      int ik = atoi(k);
      hash_elem_t *e = hashtbl_find(h, &ik);
      count--; sum -= *(int*)hash_elem_data(e);
      size_t oldsz = hashtbl_size(h);
      hashtbl_remove(h, e);
      assert(hashtbl_size(h) == oldsz - 1);
      keys = vector_remove(keys, idx);
    }
  }

  _acum_sum = 0;
  _acum_count = 0;
  hashtbl_foreach(h, (void(*)(void*))acumulate);
  assert(_acum_sum == sum);
  assert(_acum_count == (int)count);

  check_keys(h, &keys);
  vector_destroy(keys);
  return h;
}


#define ITERATIONS 1000
int main(void) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    size_t vs = 1 + random() % (2 * i);
    fprintf(stderr, "\rtesting size=%-6zu ... %3d%%", vs, 100 * i / ITERATIONS);
    hashtbl_t *h = generate_test_ht(vs, i % 2);
#ifdef _VERBOSE_
    if (i % 100 == 0) {
      fprintf(stderr, "\n");
      hashtbl_dump_stats(h);
      fprintf(stderr, "\n");
    }
#endif
    hashtbl_destroy(h);
  }
  fprintf(stderr, "\n");
  return 0;
}

/* vim: set sw=2 sts=2 : */
