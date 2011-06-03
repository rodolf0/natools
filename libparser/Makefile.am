SUBDIRS = . test

lib_LTLIBRARIES = libparser.la
libparser_la_SOURCES = \
	scanner.c \
	lexer.c \
	parser.c

libparser_includedir = $(includedir)/parser
libparser_include_HEADERS = \
	parser/scanner.h \
	parser/lexer.h \
	parser/parser.h

libparser_la_LIBADD = ../libbaas/libbaas.la
libparser_la_CPPFLAGS = -D_ERROR_CHECKING_ -I$(top_srcdir)/libbaas
libparser_la_LDFLAGS = -lm -version-info 0:1:0
