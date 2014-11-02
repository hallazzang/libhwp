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
	hwp-parser.h        \
	hwp-version.h       \
	$(NULL)

NOINST_H_FILES =        \
	gsf-input-stream.h  \
	$(NULL)

hwpincludedir = $(includedir)/libhwp
hwpinclude_HEADERS =   \
	$(INST_H_FILES)    \
	$(NULL)

BUILT_SOURCES =      \
	hwp-enum-types.h \
	hwp-enum-types.c \
	$(NULL)

libhwp_la_SOURCES =     \
	gsf-input-stream.c  \
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
	hwp-parser.c        \
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

EXTRA_DIST =                  \
	hwp-enum-types.c.template \
	hwp-enum-types.h.template \
	hwp-version.h.in          \
	$(NULL)

CLEANFILES =             \
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

if HAVE_INTROSPECTION

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS  = --add-include-path=$(srcdir) --warn-all \
                              --identifier-prefix=Hwp --symbol-prefix=hwp
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

introspection_sources = $(filter-out $(NOINST_H_FILES), gsf-input-stream.c $(libhwp_la_SOURCES))

Hwp-0.1.gir: libhwp.la
Hwp_0_1_gir_INCLUDES = GObject-2.0 Gio-2.0 cairo-1.0 Pango-1.0
Hwp_0_1_gir_CFLAGS = $(libhwp_la_CPPFLAGS) $(HWP_DEP_CFLAGS)
Hwp_0_1_gir_LIBS = libhwp.la
Hwp_0_1_gir_FILES = $(addprefix $(srcdir)/, $(introspection_sources))

INTROSPECTION_GIRS += Hwp-0.1.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)

endif
