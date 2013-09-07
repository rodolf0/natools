#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "baas/hashtbl.h"
#include "baas/vector.h"

int intcmp(const int *a, const int *b) {
  return *a > *b ? 1 : (*a < *b ? -1 : 0);
}

int totalsum;
void acumulate(int *x) {
  totalsum += *x;
}


hashtbl_t * generate_test_ht(int allow_dups) {
  size_t n = 0, i, j;
  int sum = 0, *e = NULL;
  char *rk;

  hashtbl_t *h = hashtbl_init(free, (cmp_func_t)intcmp, allow_dups);
  vector_t *v = vector_init(free, NULL);
  hash_elem_t *f;

  size_t q = random() % 17000;
  for (i = 0; i < q; i++) {
    switch (random() % 6) {
      case 0:
      case 1:
      case 2: {
        e = (int*)malloc(sizeof(int)); *e = random() % 124789;
        rk = (char*)malloc(32);
        sprintf(rk, "%d", *e);
        size_t prev_size = h->size;
        hashtbl_insert(h, rk, e);
        if (allow_dups || h->size == prev_size + 1)
          vector_append(v, rk); /* keep track of keys */
        else
          free(rk);
        n++; sum += *e;
      }
        break;
      case 3:
        if (!h->size) continue;
        rk = (char*)vector_get(v, random() % v->size);
        e = (int*)hashtbl_get(h, rk);
        assert(atoi(rk) == *e);
        break;
      case 4:
        if (!h->size) continue;
        j = random() % v->size;
        rk = (char*)vector_get(v, j);
        e = (int*)hashtbl_get(h, rk);
        n--; sum -= *e;
        hashtbl_delete(h, rk);
        vector_remove(v, j);
        break;
      case 5:
        if (!h->size) continue;
        j = random() % v->size;
        rk = (char*)vector_get(v, j);
        int k = atoi(rk);
        f = hashtbl_find(h, &k);
        n--; sum -= *(int*)f->data;
        hashtbl_remove(h, f);
        vector_remove(v, j);
        break;
    }
  }


  char **keys;
  size_t num_keys = hashtbl_keys(h, &keys);
  if (allow_dups)
    assert(num_keys == h->size);
  for (j = 0; j < num_keys; j++) {
    e = (int*)hashtbl_get(h, keys[j]);
    assert(atoi(keys[j]) == *e);
    free(keys[j]);
  }
  free(keys);

  if (allow_dups) {
    assert(n == h->size);
    totalsum = 0;
    hashtbl_foreach(h, (void(*)(void*))acumulate);
    assert(sum == totalsum);
  } else {
    while (h->size) {
      rk = (char*)vector_get(v, 0);
      hashtbl_delete(h, rk);
      assert(hashtbl_find(h, rk) == NULL);
      vector_remove(v, 0);
    }
  }

  vector_destroy(v);
  return h;
}


void check_hash_distribution(hashtbl_t *h) {
  int i;
  int *bsz = (int*)malloc(sizeof(int) * h->bktnum);

  int min = 0, max = 0, empty = 0;

  for (i = 0; (size_t)i < h->bktnum; i++) {
    if (h->buckets[i]) {
      bsz[i] = h->buckets[i]->size;
      if (bsz[i] < min || i == 0) min = bsz[i];
      if (bsz[i] > max || i == 0) max = bsz[i];
    } else
      empty++;
  }
#ifdef _DEBUG_
  fprintf(stderr,
      " bucket stats: n %lu, empty %d, min %d, max %d, total %lu, avg %f\n",
      h->bktnum, empty, min, max, h->size,
      (float)h->size / (float)(h->bktnum - empty));
#endif

  free(bsz);
}


void check_hash_function(hash_func_t hf, size_t maxkeylen) {
  size_t buckets[4096];
  const size_t nbkt = sizeof(buckets) / sizeof(size_t);
  size_t i, j;

  for (i = 0; i < nbkt; i++)
    buckets[i] = 0;

  for (i = 0; i < nbkt * 100; i++) {
    // generate a random key
    size_t keylen = 2 + random() % maxkeylen;
    unsigned char *key = (unsigned char*)malloc(keylen+1);
    for (j = 0; j < keylen; j++)
      key[j] = 1 + random() % 254;
    key[keylen] = '\0';
    // hash into bucket
    buckets[hf(key) % nbkt]++;
    free(key);
  }

  size_t min = 0, max = 0, empty = 0;
  for (i = 0; i < nbkt; i++) {
    if (buckets[i] < min || i == 0) min = buckets[i];
    if (buckets[i] > max || i == 0) max = buckets[i];
    if (buckets[i] == 0) empty++;
  }

#ifdef _DEBUG_
  fprintf(stderr,
      " bucket stats: n %lu, empty %lu, min %lu, max %lu, total %lu, avg %f\n",
      nbkt, empty, min, max, 100 * nbkt,
      (float)(100 * nbkt) / (float)(nbkt - empty));
#endif

  assert(max - min < 10 * 100*nbkt / (nbkt - empty));
  assert(empty == 0 || 100*nbkt < 10 * nbkt);
}


#ifdef _DEBUG_
void rehash_test() {
  hashtbl_t *h = hashtbl_init(NULL, NULL, 1);
  size_t i, j;
  for (i = 0; i < 1000000; i++) {
    // generate a random key
    size_t keylen = 3 + random() % 16;
    unsigned char *key = malloc(keylen+1);
    for (j = 0; j < keylen; j++)
      key[j] = 1 + random() % 254;
    key[keylen] = '\0';
    hashtbl_insert(h, (char*)key, NULL);
    free(key);
    if (i%1000)
      fprintf(stderr, "\rinserting ... %lu%%\t\t\t", 100*i/1000000);
  }
  hashtbl_destroy(h);
}
#endif

#define ITERATIONS 60
int main(void) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    hashtbl_t *h = generate_test_ht(i % 2);
#ifdef _DEBUG_
    check_hash_distribution(h);
#endif
    switch (i % 20) {
      case 0: check_hash_function(djb_hash, i); break;
      case 1: check_hash_function(sbox_hash, i); break;
      default: break;
    }
    hashtbl_destroy(h);
  }
#ifdef _DEBUG_
  rehash_test();
#endif
  fprintf(stderr, "\n");
  return 0;
}

/* vim: set sw=2 sts=2 : */
