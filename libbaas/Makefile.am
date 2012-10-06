SUBDIRS = . test

AM_CFLAGS=-O2 -Wall -Wextra -Werror -Wno-uninitialized

lib_LTLIBRARIES = libbaas.la
libbaas_la_SOURCES = \
	bignum.c \
	bits.c \
	bstree.c \
	graph.c \
	heap.c heap-list.c \
	list.c \
	sort.c sort-aux.c \
	vector.c \
	xstring.c

libbaas_includedir = $(includedir)/baas
libbaas_include_HEADERS = \
	baas/bignum.h \
	baas/bits.h \
	baas/bstree.h \
	baas/common.h \
	baas/graph.h \
	baas/heap.h baas/heap-list.h \
	baas/list.h \
	baas/sort.h \
	baas/vector.h \
	baas/xstring.h

libbaas_la_LDFLAGS = -version-info 0:1:0
