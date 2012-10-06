
bin_PROGRAMS = \
	test_list \
	test_bstree \
	test_heap \
	test_sort \
	test_bignum \
	test_bits
#	test_modarithmetic

LDADD = ../libbaas.la
AM_CFLAGS = -O0 -ggdb -Wall -Werror

test_list_SOURCES = test_list.c
test_bstree_SOURCES = test_bstree.c
test_heap_SOURCES = test_heap.c
test_sort_SOURCES = test_sort.c
test_bignum_SOURCES = test_bignum.c
test_bits_SOURCES = test_bits.c

TESTS = \
	test_list \
	test_bstree \
	test_heap \
	test_sort \
	test_bignum \
	test_bits

#test_modarithmetic_SOURCES = test_modarithmetic.c ../mod-arithmetic.c
