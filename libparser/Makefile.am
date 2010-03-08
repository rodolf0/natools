lib_LTLIBRARIES = libparser.la
libparser_la_SOURCES = \
	scanner.c scanner.h \
	lexer.c lexer.h \
	parser.c parser.h

libparser_la_CPPFLAGS = -I.. -Wall -D_ERROR_CHECKING_
libparser_la_LDFLAGS = -lm -version-info 0:1:0
libparser_la_LIBADD = ../libbaas/libbaas.la
