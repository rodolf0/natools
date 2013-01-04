add_library(baas SHARED
  bignum.c
  bits.c
  bstree.c
  graph.c
  hashtbl.c
  heap.c heap-list.c
  list.c
  sort.c sort-aux.c
  vector.c
  xstring.c
)

install(TARGETS baas LIBRARY DESTINATION lib)
install(FILES
  baas/bignum.h
  baas/bits.h
  baas/bstree.h
  baas/common.h
  baas/graph.h
  baas/hashtbl.h
  baas/heap.h baas/heap-list.h
  baas/list.h
  baas/mod-arithmetic.h
  baas/sort.h
  baas/vector.h
  baas/xstring.h
DESTINATION include/baas)


######### testing #########
enable_testing()
include_directories(BEFORE .)

add_executable(test_bignum test/test_bignum.c bignum.c)
add_executable(test_bits test/test_bits.c bits.c)
add_executable(test_bstree test/test_bstree.c bstree.c)
add_executable(test_hash test/test_hash.c hashtbl.c vector.c)
add_executable(test_heap test/test_heap.c heap.c)
add_executable(test_list test/test_list.c list.c)
add_executable(test_sort test/test_sort.c sort.c sort-aux.c)
add_executable(test_vector test/test_vector.c vector.c)

add_test(bignum test_bignum)
add_test(bits test_bits)
add_test(bstree test_bstree)
add_test(hash test_hash)
add_test(heap test_heap)
add_test(list test_list)
add_test(sort test_sort)
add_test(vector test_vector)

####### benchmarking ######
add_executable(benchmark_append test/benchmark_append.c vector.c list.c bstree.c hashtbl.c)
