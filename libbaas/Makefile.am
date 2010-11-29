lib_LTLIBRARIES = libbaas.la
libbaas_la_SOURCES = \
	list.c xstring.c sort.c sort-aux.c heap.c graph.c bits.c bignum.c 

libbaas_includedir = $(includedir)/baas
libbaas_include_HEADERS = \
	baas/list.h \
	baas/xstring.h \
	baas/sort-cmp.h \
	baas/sort.h \
	baas/heap.h \
	baas/graph.h \
	baas/bits.h \
	baas/bignum.h

libbaas_la_LDFLAGS = -version-info 0:1:0

#SUBDIRS = test
