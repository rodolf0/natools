SUBDIRS = . test

AM_CFLAGS=-O2 -Wall -Wextra -Werror -Wno-uninitialized

lib_LTLIBRARIES = libbaas.la
libbaas_la_SOURCES = \
	list.c \
	bstree.c \
	heap.c heap-list.c \
	sort.c sort-aux.c \
	graph.c \
	bits.c \
	bignum.c \
	xstring.c

libbaas_includedir = $(includedir)/baas
libbaas_include_HEADERS = \
	baas/common.h \
	baas/list.h \
	baas/bstree.h \
	baas/xstring.h \
	baas/sort.h \
	baas/heap.h baas/heap-list.h \
	baas/graph.h \
	baas/bits.h \
	baas/bignum.h

libbaas_la_LDFLAGS = -version-info 0:1:0
