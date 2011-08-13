bin_PROGRAMS = aparser roots

aparser_SOURCES = aparser.c
aparser_LDADD = -lreadline ../libparser/libparser.la
aparser_CPPFLAGS = -I$(top_srcdir)/libparser -I$(top_srcdir)/libbaas

roots_SOURCES = roots.c
roots_LDADD = ../libparser/libparser.la ../libnatools/libnatools.la
roots_CPPFLAGS = -I$(top_srcdir)/libparser \
								 -I$(top_srcdir)/libnatools -I$(top_srcdir)/libbaas
