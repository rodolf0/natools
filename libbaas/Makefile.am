AM_CFLAGS = -I$(top_srcdir) @AM_CFLAGS@

lib_LTLIBRARIES = libbaas.la
libbaas_la_SOURCES = \
	list.c list.h \
	xstring.c xstring.h \
	bits.c bits.h \
	sort.c sort-aux.c sort-cmp.h sort.h \
	bignum.c bignum.h

libbaas_la_LDFLAGS = -version-info 0:1:0

SUBDIRS = test
