## Process this file with automake to create Makefile.in.

# The name of the module.
DOC_MODULE = libhwp

# The top-level SGML file. You can change this if you want to.
DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.xml

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
IGNORE_HFILES =            \
	hwp2txt.h

GTKDOC_LIBS = $(top_builddir)/src/libhwp.la

# This includes the standard gtk-doc make rules, copied by gtkdocize.
include $(top_srcdir)/gtk-doc.make

# Other files to distribute
# e.g. EXTRA_DIST += version.xml.in
EXTRA_DIST += \
	version.xml.in
