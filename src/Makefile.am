# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libhwp.la

INST_H_FILES =          \
	hwp-charset.h       \
	hwp-document.h      \
	hwp-enums.h         \
	hwp-enum-types.h    \
	hwp-file.h          \
	hwp.h               \
	hwp-hwp3-file.h     \
	hwp-hwp3-parser.h   \
	hwp-hwp5-file.h     \
	hwp-hwp5-parser.h   \
	hwp-hwpml-file.h    \
	hwp-hwpml-parser.h  \
	hwp-listener.h      \
	hwp-models.h        \
	hwp-page.h          \
	hwp-version.h       \
	$(NULL)

NOINST_H_FILES =        \
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
	hwp-charset.c       \
	hwp-document.c      \
	hwp-enums.c         \
	hwp-enum-types.c    \
	hwp-file.c          \
	hwp-hwp3-file.c     \
	hwp-hwp3-parser.c   \
	hwp-hwp5-file.c     \
	hwp-hwp5-parser.c   \
	hwp-hwpml-file.c    \
	hwp-hwpml-parser.c  \
	hwp-listener.c      \
	hwp-models.c        \
	hwp-page.c          \
	$(NOINST_H_FILES)   \
	$(INST_H_FILES)     \
	$(NULL)

libhwp_la_CPPFLAGS =        \
	-DG_LOG_DOMAIN=\"HWP\"  \
	-DHWP_COMPILATION       \
	$(AM_CPPFLAGS)          \
	$(NULL)

libhwp_la_CFLAGS =      \
	-Wall               \
	$(HWP_DEP_CFLAGS)   \
	$(HWP_DEBUG_FLAGS)  \
	$(AM_CFLAGS)        \
	$(NULL)

libhwp_la_LDFLAGS =                      \
	-version-info $(HWP_LT_VERSION_INFO) \
	-no-undefined                        \
	-export-symbols-regex "^hwp_*"       \
	$(AM_LDFLAGS)                        \
	$(NULL)

libhwp_la_LIBADD = $(HWP_DEP_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = hwp-0.0.4.pc

EXTRA_DIST =                  \
	hwp-0.0.4.pc.in           \
	hwp-enum-types.c.template \
	hwp-enum-types.h.template \
	hwp-version.h.in          \
	$(NULL)

DISTCLEANFILES =     \
	hwp-0.0.4.pc     \
	hwp-enum-types.c \
	hwp-enum-types.h \
	hwp-version.h    \
	$(NULL)

hwp_headers = $(filter-out hwp-enum-types.h, $(INST_H_FILES))

hwp-enum-types.h: $(hwp_headers) hwp-enum-types.h.template
	$(AM_V_GEN) glib-mkenums --identifier-prefix Hwp --template \
	    hwp-enum-types.h.template $(hwp_headers) > hwp-enum-types.h

hwp-enum-types.c: $(hwp_headers) hwp-enum-types.c.template
	$(AM_V_GEN) glib-mkenums --identifier-prefix Hwp --template \
	    hwp-enum-types.c.template $(hwp_headers) > hwp-enum-types.c

bin_PROGRAMS = hwp2pdf hwp2svg hwp2txt unhwp

hwp2pdf_SOURCES = hwp2pdf.c
hwp2pdf_CFLAGS  = $(HWP_DEP_CFLAGS)
hwp2pdf_LDADD   = libhwp.la $(HWP_DEP_LIBS)

hwp2svg_SOURCES = hwp2svg.c
hwp2svg_CFLAGS  = $(HWP_DEP_CFLAGS)
hwp2svg_LDADD   = libhwp.la $(HWP_DEP_LIBS)

hwp2txt_SOURCES = hwp2txt.c hwp2txt.h
hwp2txt_CFLAGS  = $(HWP_DEP_CFLAGS)
hwp2txt_LDADD   = libhwp.la $(HWP_DEP_LIBS)

unhwp_SOURCES   = unhwp.c
unhwp_CFLAGS    = $(HWP_DEP_CFLAGS)
unhwp_LDADD     = libhwp.la $(HWP_DEP_LIBS)

CLEANFILES = $(bin_PROGRAMS) $(gir_DATA) $(typelib_DATA)

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS  = --add-include-path=$(srcdir) --warn-all \
                              --identifier-prefix=Hwp --symbol-prefix=hwp
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_sources = $(filter-out $(NOINST_H_FILES), $(libhwp_la_SOURCES))

Hwp-0.0.gir: libhwp.la
Hwp_0_0_gir_INCLUDES = GObject-2.0 Gio-2.0 cairo-1.0
Hwp_0_0_gir_CFLAGS = $(libhwp_la_CPPFLAGS) $(HWP_DEP_CFLAGS)
Hwp_0_0_gir_LIBS = libhwp.la
Hwp_0_0_gir_FILES = $(addprefix $(srcdir)/, $(introspection_sources))

INTROSPECTION_GIRS += Hwp-0.0.gir

girdir = $(datadir)/gir-1.2
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

endif
