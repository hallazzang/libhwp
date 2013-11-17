## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libhwp.la

hwp_headers =               \
	hwp-context-v3.h    \
	hwp-document.h      \
	hwp-enums.h         \
	hwp-file.h          \
	hwp-file-ml.h       \
	hwp-file-v3.h       \
	hwp-file-v5.h       \
	hwp.h               \
	hwp-models.h        \
	hwp-page.h          \
	hwp-listener.h      \
	hwp-parser.h        \
	hwp-version.h       \
	gsf-input-stream.h   \
	hnc2unicode.h        \
	$(NULL)

NOINST_H_FILES = \
	$(NULL)

INST_H_FILES =            \
	$(hwp_headers)   \
	hwp-enum-types.h \
	$(NULL)

hwpincludedir = $(includedir)/hwp
hwpinclude_HEADERS =   \
	$(INST_H_FILES) \
	$(NULL)

BUILT_SOURCES =           \
	hwp-enum-types.h \
	hwp-enum-types.c \
	$(NULL)

libhwp_la_SOURCES =         \
	hwp-context-v3.c    \
	hwp-document.c      \
	hwp-enums.c         \
	hwp-file.c          \
	hwp-file-ml.c       \
	hwp-file-v3.c       \
	hwp-file-v5.c       \
	hwp-models.c        \
	hwp-page.c          \
	hwp-listener.c      \
	hwp-parser.c        \
	gsf-input-stream.c   \
	hnc2unicode.c        \
	hwp-enum-types.c    \
	$(NOINST_H_FILES)    \
	$(INST_H_FILES)      \
	$(NULL)

libhwp_la_CPPFLAGS =           \
	-DG_LOG_DOMAIN=\"HWP\" \
	-DHWP_COMPILATION      \
	$(AM_CPPFLAGS)          \
	$(NULL)

libhwp_la_CFLAGS =     \
	$(HWP_CFLAGS)  \
	-Wall           \
	$(AM_CFLAGS)    \
	$(NULL)

libhwp_la_LDFLAGS =                          \
	-version-info $(HWP_LT_VERSION_INFO) \
	-no-undefined                         \
	-export-symbols-regex "^hwp_*"       \
	$(AM_LDFLAGS)                         \
	$(NULL)

libhwp_la_LIBADD = $(HWP_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = hwp-0.2.pc

EXTRA_DIST =                       \
	hwp-0.2.pc.in             \
	hwp-version.h.in          \
	hwp-enum-types.h.template \
	hwp-enum-types.c.template \
	$(NULL)

DISTCLEANFILES =          \
	hwp-0.2.pc       \
	hwp-version.h    \
	hwp-enum-types.h \
	hwp-enum-types.c \
	$(NULL)

hwp-enum-types.h: $(hwp_headers) hwp-enum-types.h.template
	$(AM_V_GEN) glib-mkenums --identifier-prefix HWP --template \
	    hwp-enum-types.h.template $(hwp_headers) > hwp-enum-types.h

hwp-enum-types.c: $(hwp_headers) hwp-enum-types.c.template
	$(AM_V_GEN) glib-mkenums --identifier-prefix HWP --template \
	    hwp-enum-types.c.template $(hwp_headers) > hwp-enum-types.c

bin_PROGRAMS = hwp2pdf hwp2svg unhwp

hwp2pdf_SOURCES = hwp2pdf.c
hwp2pdf_CFLAGS  = $(HWP_CFLAGS)
hwp2pdf_LDADD   = libhwp.la

hwp2svg_SOURCES = hwp2svg.c
hwp2svg_CFLAGS  = $(HWP_CFLAGS)
hwp2svg_LDADD   = libhwp.la

unhwp_SOURCES   = unhwp.c
unhwp_CFLAGS    = $(HWP_CFLAGS)
unhwp_LDADD     = -lgsf-1
