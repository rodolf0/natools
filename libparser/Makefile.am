SUBDIRS = . test

AM_CFLAGS=-O2 -Wall -Wextra -Werror -Wno-uninitialized -D_VERBOSE_

lib_LTLIBRARIES = libparser.la
libparser_la_SOURCES = \
	scanner.c \
	lexer.c lexer-ids.c lexer-operators.c \
	parser.c parser-1.c semanter-1.c functions-1.c

libparser_includedir = $(includedir)/parser
libparser_include_HEADERS = \
	parser/scanner.h \
	parser/lexer.h \
	parser/parser.h parser-priv.h

libparser_la_LIBADD = ../libbaas/libbaas.la
libparser_la_CPPFLAGS = -I$(top_srcdir)/libbaas
libparser_la_LDFLAGS = -lm -version-info 0:1:0
