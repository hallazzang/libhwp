## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libghwp.la

ghwp_headers =         \
	ghwp-context-v3.h  \
	ghwp-document.h    \
	ghwp-enums.h       \
	ghwp-file.h        \
	ghwp-file-ml.h     \
	ghwp-file-v3.h     \
	ghwp-file-v5.h     \
	ghwp.h             \
	ghwp-models.h      \
	ghwp-page.h        \
	ghwp-parse.h       \
	ghwp-version.h     \
	gsf-input-stream.h \
	hnc2unicode.h      \
	$(NULL)

NOINST_H_FILES = \
	$(NULL)

INST_H_FILES =         \
	$(ghwp_headers)    \
	ghwp-enum-types.h  \
	$(NULL)

ghwpincludedir = $(includedir)/ghwp
ghwpinclude_HEADERS =  \
	$(INST_H_FILES)    \
	$(NULL)

BUILT_SOURCES =       \
	ghwp-enum-types.h \
	ghwp-enum-types.c \
	$(NULL)

libghwp_la_SOURCES =   \
	ghwp-context-v3.c  \
	ghwp-document.c    \
	ghwp-enums.c       \
	ghwp-file.c        \
	ghwp-file-ml.c     \
	ghwp-file-v3.c     \
	ghwp-file-v5.c     \
	ghwp-models.c      \
	ghwp-page.c        \
	ghwp-parse.c       \
	gsf-input-stream.c \
	hnc2unicode.c      \
	ghwp-enum-types.c  \
	$(NOINST_H_FILES)  \
	$(INST_H_FILES)    \
	$(NULL)

libghwp_la_CPPFLAGS =       \
	-DG_LOG_DOMAIN=\"GHWP\" \
	-DGHWP_COMPILATION      \
	$(AM_CPPFLAGS)          \
	$(NULL)

libghwp_la_CFLAGS = \
	$(GHWP_CFLAGS)  \
	-Wall           \
	$(AM_CFLAGS)    \
	$(NULL)

libghwp_la_LDFLAGS =                      \
	-version-info $(GHWP_LT_VERSION_INFO) \
	-no-undefined                         \
	-export-symbols-regex "^ghwp_*"       \
	$(AM_LDFLAGS)                         \
	$(NULL)

libghwp_la_LIBADD = $(GHWP_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ghwp-0.2.pc

EXTRA_DIST =                   \
	ghwp-0.2.pc.in             \
	ghwp-version.h.in          \
	ghwp-enum-types.h.template \
	ghwp-enum-types.c.template \
	$(NULL)

DISTCLEANFILES =      \
	ghwp-0.2.pc       \
	ghwp-version.h    \
	ghwp-enum-types.h \
	ghwp-enum-types.c \
	$(NULL)

ghwp-enum-types.h: $(ghwp_headers) ghwp-enum-types.h.template
	$(AM_V_GEN) glib-mkenums --identifier-prefix GHWP --template \
	    ghwp-enum-types.h.template $(ghwp_headers) > ghwp-enum-types.h

ghwp-enum-types.c: $(ghwp_headers) ghwp-enum-types.c.template
	$(AM_V_GEN) glib-mkenums --identifier-prefix GHWP --template \
	    ghwp-enum-types.c.template $(ghwp_headers) > ghwp-enum-types.c

bin_PROGRAMS = hwp2pdf hwp2svg unhwp

hwp2pdf_SOURCES = hwp2pdf.c
hwp2pdf_CFLAGS  = $(GHWP_CFLAGS)
hwp2pdf_LDADD   = libghwp.la

hwp2svg_SOURCES = hwp2svg.c
hwp2svg_CFLAGS  = $(GHWP_CFLAGS)
hwp2svg_LDADD   = libghwp.la

unhwp_SOURCES   = unhwp.c
unhwp_CFLAGS    = $(GHWP_CFLAGS)
unhwp_LDADD     = -lgsf-1
