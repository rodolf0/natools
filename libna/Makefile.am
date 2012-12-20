SUBDIRS = . test

AM_CFLAGS=-O2 -Wall -Wextra -Werror -Wno-uninitialized

lib_LTLIBRARIES = libna.la
libna_la_SOURCES = \
	function.c \
	combinatorics.c   \
	calculus.c \
	root_finding.c \
	interpolation.c \
	natools.c

libna_includedir = $(includedir)/na
libna_include_HEADERS = \
	na/function.h \
	na/combinatorics.h \
 	na/calculus.h \
	na/natools.h \
	na/root_finding.h \
	na/interpolation.h

libna_la_LIBADD = ../libparser/libparser.la
libna_la_CFLAGS = $(AM_CFLAGS) -I$(top_srcdir)/libparser -I$(top_srcdir)/libbaas
libna_la_LDFLAGS = -lm -version-info 0:1:0
