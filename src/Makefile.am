# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libhwp.la

INST_H_FILES =          \
	hwp-document.h      \
	hwp-enums.h         \
	hwp-enum-types.h    \
	hwp-file.h          \
	hwp-file-ml.h       \
	hwp-file-v3.h       \
	hwp-file-v5.h       \
	hwp.h               \
	hwp-models.h        \
	hwp-page.h          \
	hwp-listener.h      \
	hwp-hwp3-parser.h   \
	hwp-hwp5-parser.h   \
	hwp-version.h       \
	hnc2unicode.h       \
	$(NULL)

NOINST_H_FILES =        \
	gsf-input-stream.h  \
	$(NULL)

hwpincludedir = $(includedir)/hwp
hwpinclude_HEADERS =   \
	$(INST_H_FILES)    \
	$(NULL)

BUILT_SOURCES =      \
	hwp-enum-types.h \
	hwp-enum-types.c \
	$(NULL)

libhwp_la_SOURCES =     \
	hwp-document.c      \
	hwp-enums.c         \
	hwp-file.c          \
	hwp-file-ml.c       \
	hwp-file-v3.c       \
	hwp-file-v5.c       \
	hwp-models.c        \
	hwp-page.c          \
	hwp-listener.c      \
	hwp-hwp3-parser.c   \
	hwp-hwp5-parser.c   \
	gsf-input-stream.c  \
	hnc2unicode.c       \
	hwp-enum-types.c    \
	$(NOINST_H_FILES)   \
	$(INST_H_FILES)     \
	$(NULL)

libhwp_la_CPPFLAGS =        \
	-DG_LOG_DOMAIN=\"HWP\"  \
	-DHWP_COMPILATION       \
	$(AM_CPPFLAGS)          \
	$(NULL)

libhwp_la_CFLAGS =  \
	$(HWP_CFLAGS)   \
	-Wall           \
	$(AM_CFLAGS)    \
	$(NULL)

libhwp_la_LDFLAGS =                      \
	-version-info $(HWP_LT_VERSION_INFO) \
	-no-undefined                        \
	-export-symbols-regex "^hwp_*"       \
	$(AM_LDFLAGS)                        \
	$(NULL)

libhwp_la_LIBADD = $(HWP_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = hwp-0.2.pc

EXTRA_DIST =                  \
	hwp-0.2.pc.in             \
	hwp-version.h.in          \
	hwp-enum-types.h.template \
	hwp-enum-types.c.template \
	$(NULL)

DISTCLEANFILES =     \
	hwp-0.2.pc       \
	hwp-version.h    \
	hwp-enum-types.h \
	hwp-enum-types.c \
	$(NULL)

hwp_headers = $(filter-out hwp-enum-types.h, $(INST_H_FILES))

hwp-enum-types.h: $(hwp_headers) hwp-enum-types.h.template
	$(AM_V_GEN) glib-mkenums --identifier-prefix HWP --template \
	    hwp-enum-types.h.template $(hwp_headers) > hwp-enum-types.h

hwp-enum-types.c: $(hwp_headers) hwp-enum-types.c.template
	$(AM_V_GEN) glib-mkenums --identifier-prefix HWP --template \
	    hwp-enum-types.c.template $(hwp_headers) > hwp-enum-types.c

bin_PROGRAMS = hwp2pdf hwp2svg unhwp

hwp2pdf_SOURCES = hwp2pdf.c
hwp2pdf_CFLAGS  = $(HWP_CFLAGS)
hwp2pdf_LDADD   = libhwp.la $(HWP_LIBS)

hwp2svg_SOURCES = hwp2svg.c
hwp2svg_CFLAGS  = $(HWP_CFLAGS)
hwp2svg_LDADD   = libhwp.la $(HWP_LIBS)

unhwp_SOURCES   = unhwp.c
unhwp_CFLAGS    = $(HWP_CFLAGS)
unhwp_LDADD     = libhwp.la $(HWP_LIBS)

CLEANFILES = $(gir_DATA) $(typelib_DATA)

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS  = --add-include-path=$(srcdir) --warn-all \
                              --identifier-prefix=HWP --symbol-prefix=hwp
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_sources = $(filter-out $(NOINST_H_FILES) gsf-input-stream.c, $(libhwp_la_SOURCES))

HWP-0.2.gir: libhwp.la
HWP_0_2_gir_INCLUDES = GObject-2.0 Gio-2.0 cairo-1.0 Pango-1.0
HWP_0_2_gir_CFLAGS = $(libhwp_la_CPPFLAGS) $(HWP_CFLAGS)
HWP_0_2_gir_LIBS = libhwp.la
HWP_0_2_gir_FILES = $(addprefix $(srcdir)/, $(introspection_sources))

INTROSPECTION_GIRS += HWP-0.2.gir

girdir = $(datadir)/gir-1.2
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

endif