lib_LTLIBRARIES = libhwp.la

INST_H_FILES =          \
	hwp-charset.h       \
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
	hwp-listenable.h    \
	hwp-models.h        \
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
	hwp-enums.c         \
	hwp-enum-types.c    \
	hwp-file.c          \
	hwp-hwp3-file.c     \
	hwp-hwp3-parser.c   \
	hwp-hwp5-file.c     \
	hwp-hwp5-parser.c   \
	hwp-hwpml-file.c    \
	hwp-hwpml-parser.c  \
	hwp-listenable.c    \
	hwp-models.c        \
	hwp-parser.c        \
	$(NOINST_H_FILES)   \
	$(INST_H_FILES)     \
	$(NULL)

libhwp_la_CFLAGS = \
	-Wall -Werror \
	-DG_LOG_DOMAIN=\"HWP\" \
	-DHWP_COMPILATION \
	$(LIBHWP_DEPS_CFLAGS) \
	$(HWP_DEBUG_FLAGS) \
	$(NULL)

libhwp_la_LDFLAGS = \
	-version-info $(HWP_LT_VERSION_INFO) \
	-no-undefined \
	-export-symbols-regex "^hwp_*" \
	$(LIBHWP_DEPS_LIBS) \
	$(NULL)

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

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS  = --add-include-path=$(srcdir) \
                              --warn-all --warn-error \
                              --identifier-prefix=Hwp --symbol-prefix=hwp
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

introspection_sources = $(filter-out $(NOINST_H_FILES), gsf-input-stream.c $(libhwp_la_SOURCES))

Hwp-2016.05.gir: libhwp.la
Hwp_2016_05_gir_INCLUDES = GObject-2.0 Gio-2.0 cairo-1.0 Pango-1.0
Hwp_2016_05_gir_CFLAGS = $(libhwp_la_CFLAGS)
Hwp_2016_05_gir_LIBS = libhwp.la
Hwp_2016_05_gir_FILES = $(addprefix $(srcdir)/, $(introspection_sources))

INTROSPECTION_GIRS += Hwp-2016.05.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)

DISTCLEANFILES = Makefile.in
