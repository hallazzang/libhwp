# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src

bin_PROGRAMS = hwp2pdf hwp2svg hwp2txt unhwp

man_MANS = hwp2pdf.1 hwp2svg.1 hwp2txt.1 unhwp.1

hwp2pdf_SOURCES = hwp2pdf.c
hwp2pdf_CFLAGS  = $(HWP_DEP_CFLAGS)
hwp2pdf_LDADD   = $(top_srcdir)/src/libhwp.la $(HWP_DEP_LIBS)

hwp2svg_SOURCES = hwp2svg.c
hwp2svg_CFLAGS  = $(HWP_DEP_CFLAGS)
hwp2svg_LDADD   = $(top_srcdir)/src/libhwp.la $(HWP_DEP_LIBS)

hwp2txt_SOURCES = hwp2txt.c
hwp2txt_CFLAGS  = $(HWP_DEP_CFLAGS)
hwp2txt_LDADD   = $(top_srcdir)/src/libhwp.la $(HWP_DEP_LIBS)

unhwp_SOURCES   = unhwp.c
unhwp_CFLAGS    = $(HWP_DEP_CFLAGS)
unhwp_LDADD     = $(top_srcdir)/src/libhwp.la $(HWP_DEP_LIBS)

CLEANFILES = $(bin_PROGRAMS)
