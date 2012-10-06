AM_CFLAGS=-O2 -Wall -Wextra -Werror -Wno-uninitialized

lib_LTLIBRARIES = libnatools.la
libnatools_la_SOURCES = natools.c combinatorics.c function.c  \
										  equation_solver.c interpolation.c calculus.c 

libnatools_includedir = $(includedir)/natools
libnatools_include_HEADERS = natools/natools.h natools/combinatorics.h \
													 natools/function.h natools/equation_solver.h \
													 natools/interpolation.h natools/calculus.h

libnatools_la_LIBADD = ../libparser/libparser.la
libnatools_la_CFLAGS = $(AM_CFLAGS) -I$(top_srcdir)/libparser -I$(top_srcdir)/libbaas
libnatools_la_LDFLAGS = -lm -version-info 0:1:0
