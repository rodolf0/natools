bin_PROGRAMS = aparser roots

AM_CFLAGS=-O2 -Wall -Wextra -Werror -Wno-uninitialized

aparser_SOURCES = aparser.c
aparser_LDADD = -lreadline ../libparser/libparser.la ../libbaas/libbaas.la
aparser_CFLAGS = $(AM_CFLAGS) -I$(top_srcdir)/libparser -I$(top_srcdir)/libbaas

roots_SOURCES = roots.c
roots_LDADD = ../libparser/libparser.la ../libnatools/libnatools.la
roots_CFLAGS = \
	$(AM_CFLAGS) -I$(top_srcdir)/libparser \
	-I$(top_srcdir)/libnatools -I$(top_srcdir)/libbaas
