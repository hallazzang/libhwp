# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src

bin_PROGRAMS = hwp2txt unhwp

man_MANS = hwp2txt.1 unhwp.1

hwp2txt_SOURCES = hwp2txt.c
hwp2txt_CFLAGS  = $(LIBHWP_DEPS_CFLAGS)
hwp2txt_LDADD   = $(top_srcdir)/src/libhwp.la $(LIBHWP_DEPS_LIBS)

unhwp_SOURCES   = unhwp.c
unhwp_CFLAGS    = $(LIBHWP_DEPS_CFLAGS)
unhwp_LDADD     = $(top_srcdir)/src/libhwp.la $(LIBHWP_DEPS_LIBS)

CLEANFILES = $(bin_PROGRAMS)

DISTCLEANFILES = Makefile.in
