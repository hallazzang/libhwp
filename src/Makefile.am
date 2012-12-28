## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libghwp.la


libghwp_la_SOURCES = \
	ghwp-document.c \
	ghwp-file.c \
	ghwp-page.c \
	ghwp-parse.c \
	gsf-input-stream.c

libghwp_la_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"GHWP\" \
	-DGHWP_COMPILATION      \
	$(AM_CPPFLAGS)

libghwp_la_CFLAGS = \
	$(GHWP_CFLAGS) \
	-Wall             \
	$(AM_CFLAGS)

libghwp_la_LDFLAGS = \
	-version-info $(GHWP_LT_VERSION_INFO) \
	-no-undefined                         \
	-export-symbols-regex "^ghwp_*"       \
	$(AM_LDFLAGS)

libghwp_la_LIBADD = $(GHWP_LIBS)

ghwpincludedir = $(includedir)/ghwp

ghwpinclude_HEADERS = \
	ghwp.h          \
	ghwp-document.h \
	ghwp-file.h     \
	ghwp-page.h     \
	ghwp-parse.h    \
	ghwp-tags.h     \
	ghwp-version.h  \
	gsf-input-stream.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ghwp-0.1.pc

EXTRA_DIST = \
	ghwp-0.1.pc.in \
	ghwp-version.h.in

DISTCLEANFILES = \
	ghwp-0.1.pc \
	ghwp-version.h