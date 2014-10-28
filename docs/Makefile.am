## Process this file with automake to create Makefile.in.

# The name of the module.
DOC_MODULE = libhwp

# The top-level SGML file.
DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.xml

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS = --rebuild-types

# The directory containing the source code. Relative to $(srcdir).
# gtk-doc will search all .c & .h files beneath here for inline comments
# documenting the functions and macros.
# e.g. DOC_SOURCE_DIR=../../../gtk
DOC_SOURCE_DIR = $(top_srcdir)/src

# Used for dependencies. The docs will be rebuilt if any of these change.
# e.g. HFILE_GLOB=$(top_srcdir)/gtk/*.h
# e.g. CFILE_GLOB=$(top_srcdir)/gtk/*.c
HFILE_GLOB = $(top_srcdir)/src/*.h
CFILE_GLOB = $(top_srcdir)/src/*.c

# Header files to ignore when scanning. Use base file name, no paths
# e.g. IGNORE_HFILES=gtkdebug.h gtkintl.h
IGNORE_HFILES =

# CFLAGS and LDFLAGS for compiling gtkdoc-scangobj with your library.
# Only needed if you are using gtkdoc-scangobj to dynamically query widget
# signals and properties.
# e.g. GTKDOC_CFLAGS=-I$(top_srcdir) -I$(top_builddir) $(GTK_DEBUG_FLAGS)
# e.g. GTKDOC_LIBS=$(top_builddir)/gtk/$(gtktargetlib)
GTKDOC_CFLAGS = $(HWP_DEP_CFLAGS)
GTKDOC_LIBS = $(top_builddir)/src/libhwp.la

# This includes the standard gtk-doc make rules, copied by gtkdocize.
include $(top_srcdir)/gtk-doc.make

# Other files to distribute
# e.g. EXTRA_DIST += version.xml.in
EXTRA_DIST += \
	version.xml.in

# Files not to distribute
# for --rebuild-types in $(SCAN_OPTIONS), e.g. $(DOC_MODULE).types
# for --rebuild-sections in $(SCAN_OPTIONS) e.g. $(DOC_MODULE)-sections.txt
#DISTCLEANFILES +=

CLEANFILES += \
	$(DOC_MODULE)-decl-list.txt \
	$(DOC_MODULE)-decl.txt \
	$(DOC_MODULE)-overrides.txt \
	$(DOC_MODULE)-sections.txt \
	$(DOC_MODULE)-undeclared.txt \
	$(DOC_MODULE)-undocumented.txt \
	$(DOC_MODULE)-unused.txt \
	$(DOC_MODULE).args \
	$(DOC_MODULE).hierarchy \
	$(DOC_MODULE).interfaces \
	$(DOC_MODULE).prerequisites \
	$(DOC_MODULE).signals \
	$(DOC_MODULE).types \
	*.stamp \
	-r html xml
