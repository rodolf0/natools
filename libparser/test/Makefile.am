bin_PROGRAMS = \
	test_scanner \
	test_lexer \
	test_parser2

LDADD = ../libparser.la ../../libbaas/libbaas.la
AM_CFLAGS = -O0 -ggdb -Wall -Werror -I$(top_srcdir)/libbaas

test_scanner_SOURCES = test_scanner.c
test_lexer_SOURCES = test_lexer.c
test_parser2_SOURCES = test_parser2.c

TESTS = \
	test_scanner \
	test_lexer \
	test_parser2
