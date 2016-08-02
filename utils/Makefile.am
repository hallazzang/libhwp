bin_PROGRAMS = hwp2txt unhwp

man_MANS = hwp2txt.1 unhwp.1

AM_CFLAGS = \
	-Wall -Werror \
	-I$(top_srcdir)/src

hwp2txt_SOURCES = hwp2txt.c
hwp2txt_CFLAGS  = $(HWP2TXT_DEPS_CFLAGS) $(AM_CFLAGS)
hwp2txt_LDFLAGS = $(HWP2TXT_DEPS_LIBS)
hwp2txt_LDADD   = $(top_srcdir)/src/libhwp.la

unhwp_SOURCES   = unhwp.c
unhwp_CFLAGS    = $(UNHWP_DEPS_CFLAGS) $(AM_CFLAGS)
unhwp_LDFLAGS   = $(UNHWP_DEPS_LIBS)
unhwp_LDADD     = $(top_srcdir)/src/libhwp.la

CLEANFILES = $(bin_PROGRAMS)

DISTCLEANFILES = Makefile.in
