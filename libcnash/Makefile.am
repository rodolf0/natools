lib_LTLIBRARIES = libcnash.la
libcnash_la_SOURCES = \
	cnash.c cnash.h \
	function.c function.h \
	root_search.c root_search.h

libcnash_la_CPPFLAGS = -I.. -Wall
libcnash_la_LDFLAGS = -lm -version-info 0:1:0
libcnash_la_LIBADD = ../libparser/libparser.la
