add_library(baas SHARED
  bignum.c
  bits.c
  bstree.c
  hashtbl.c
  heap.c
  list.c
  memory.c
  sort.c sort-aux.c
  vector.c
  xstring.c
)
set_target_properties(baas PROPERTIES COMPILE_FLAGS "-DNDEBUG -O2")

install(TARGETS baas LIBRARY DESTINATION lib)
install(FILES
  baas/bignum.h
  baas/bits.h
  baas/bstree.h
  baas/common.h
  baas/hashtbl.h
  baas/heap.h
  baas/list.h
  baas/mod-arithmetic.h
  baas/sort.h
  baas/vector.h
  baas/xstring.h
DESTINATION include/baas)


######### testing #########
enable_testing()
include_directories(BEFORE .)

add_executable(test_bignum test/test_bignum.c bignum.c memory.c)
add_executable(test_bits test/test_bits.c bits.c)
add_executable(test_bstree test/test_bstree.c bstree.c memory.c)
add_executable(test_hash test/test_hash.c hashtbl.c vector.c memory.c)
add_executable(test_heap test/test_heap.c heap.c memory.c)
add_executable(test_list test/test_list.c list.c memory.c)
add_executable(test_sort test/test_sort.c sort.c sort-aux.c memory.c)
add_executable(test_vector test/test_vector.c vector.c memory.c)
add_executable(benchmark_append test/benchmark_append.c vector.c list.c bstree.c hashtbl.c memory.c)

set_target_properties(
  test_bignum
  test_bits
  test_bstree
  test_hash
  test_heap
  test_list
  test_sort
  test_vector
  benchmark_append
  PROPERTIES COMPILE_FLAGS "-DDEBUG -ggdb -O0")

add_test(bignum test_bignum)
add_test(bits test_bits)
add_test(bstree test_bstree)
add_test(hash test_hash)
add_test(heap test_heap)
add_test(list test_list)
add_test(sort test_sort)
add_test(vector test_vector)
