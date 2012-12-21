bin_PROGRAMS = \
	test_scanner \
	test_lexer \
	test_parser

LDADD = ../libparser.la ../../libbaas/libbaas.la
AM_CFLAGS = -O0 -ggdb -Wall -Werror -I$(top_srcdir)/libbaas

test_scanner_SOURCES = test_scanner.c
test_lexer_SOURCES = test_lexer.c
test_parser_SOURCES = test_parser.c

TESTS = \
	test_scanner \
	test_lexer \
	test_parser
