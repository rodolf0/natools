lib_LTLIBRARIES = libcnash.la
libcnash_la_SOURCES = cnash.c combinatorics.c function.c  \
										  equation_solver.c interpolation.c calculus.c 

libcnash_includedir = $(includedir)/cnash
libcnash_include_HEADERS = cnash/cnash.h cnash/combinatorics.h \
													 cnash/function.h cnash/equation_solver.h \
													 cnash/interpolation.h cnash/calculus.h

libcnash_la_LIBADD = ../libparser/libparser.la
libcnash_la_CPPFLAGS = -I$(top_srcdir)/libparser -I$(top_srcdir)/libbaas
libcnash_la_LDFLAGS = -lm -version-info 0:1:0
