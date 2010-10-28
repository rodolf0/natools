AM_CFLAGS = -I$(top_srcdir) @AM_CFLAGS@

lib_LTLIBRARIES = libcnash.la

libcnash_la_SOURCES = \
	cnash.c cnash.h \
	combinatorics.c combinatorics.h \
	function.c function.h \
	equation_solver.c equation_solver.h \
	interpolation.c interpolation.h \
	calculus.c calculus.h

libcnash_la_LDFLAGS = -lm -version-info 0:1:0
libcnash_la_LIBADD = ../libparser/libparser.la
