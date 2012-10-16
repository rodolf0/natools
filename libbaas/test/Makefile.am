
bin_PROGRAMS = \
	test_bignum \
	test_bits \
	test_bstree \
	test_hash \
	test_heap \
	test_list \
	test_sort \
	test_vector
#	test_modarithmetic

LDADD = ../libbaas.la
AM_CFLAGS = -O0 -ggdb -Wall -Werror

test_bignum_SOURCES = test_bignum.c
test_bits_SOURCES = test_bits.c
test_bstree_SOURCES = test_bstree.c
test_hash_SOURCES = test_hash.c
test_heap_SOURCES = test_heap.c
test_list_SOURCES = test_list.c
test_sort_SOURCES = test_sort.c
test_vector_SOURCES = test_vector.c

TESTS = \
	test_bignum \
	test_bits \
	test_bstree \
	test_heap \
	test_hash \
	test_list \
	test_sort \
	test_vector

#test_modarithmetic_SOURCES = test_modarithmetic.c ../mod-arithmetic.c
