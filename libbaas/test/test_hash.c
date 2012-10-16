#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../baas/hashtbl.h"
#include "../baas/vector.h"

int intcmp(const int *a, const int *b) {
  return *a > *b ? 1 : (*a < *b ? -1 : 0);
}

int totalsum;
void acumulate(int *x) {
  totalsum += *x;
}


hashtbl_t * generate_test_ht() {
  int n = 0, sum = 0, *e = NULL, i, j;
  char *rk;

  hashtbl_t *h = hashtbl_init(free);
  vector_t *v = vector_init(free, NULL);

  int q = random() % 17000;
  for (i = 0; i < q; i++) {
    switch (random() % 4) {
      case 0:
      case 1:
        e = malloc(sizeof(int)); *e = random() % 124789;
        rk = (char*)malloc(32);
        sprintf(rk, "%d", *e);
        vector_append(v, rk); /* keep track of keys */
        n++; sum += *e;
        hashtbl_insert(h, rk, e);
        break;
      case 2:
        if (!h->size) continue;
        rk = vector_get(v, random() % v->size);
        e = hashtbl_get(h, rk);
        assert(atoi(rk) == *e);
        break;
      case 3:
        if (!h->size) continue;
        j = random() % v->size;
        rk = vector_get(v, j);
        e = hashtbl_get(h, rk);
        n--; sum -= *e;
        hashtbl_remove(h, rk);
        vector_remove(v, j);
        break;
    }
  }


  char **keys;
  size_t num_keys = hashtbl_keys(h, &keys);
  assert(num_keys == h->size);
  for (j = 0; j < num_keys; j++) {
    e = hashtbl_get(h, keys[j]);
    assert(atoi(keys[j]) == *e);
    free(keys[j]);
  }
  free(keys);

  assert(n == h->size);
  totalsum = 0;
  hashtbl_foreach(h, (void(*)(void*))acumulate);
  assert(sum == totalsum);

  vector_destroy(v);
  return h;
}


void check_hash_distribution(hashtbl_t *h) {
  int i;
  int *bsz = malloc(sizeof(int) * h->bktnum);

  int min = 0, max = 0, empty = 0;

  for (i = 0; i < h->bktnum; i++) {
    if (h->buckets[i]) {
      bsz[i] = h->buckets[i]->size;
      if (bsz[i] < min || i == 0) min = bsz[i];
      if (bsz[i] > max || i == 0) max = bsz[i];
    } else
      empty++;
  }
  fprintf(stderr,
      " bucket stats: n %lu, empty %d, min %d, max %d, total %lu, avg %f\n",
      h->bktnum, empty, min, max, h->size,
      (float)h->size / (float)(h->bktnum - empty));

  free(bsz);
  assert(empty == 0 || h->size < 5 * h->bktnum);
}


#define ITERATIONS 100
int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i <= ITERATIONS; i++) {
    fprintf(stderr, "\rtesting ... %d%%", 100*i/ITERATIONS);
    hashtbl_t *h = generate_test_ht();
    check_hash_distribution(h);
    hashtbl_destroy(h);
  }
  fprintf(stderr, "\n");
  return 0;
}

/* vim: set sw=2 sts=2 : */


/* vim: set sw=2 sts=2 : */
