SUBDIRS = src utils docs po

ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libhwp.pc

EXTRA_DIST = libhwp.pc.in

DISTCLEANFILES = \
	Makefile.in \
	po/Makefile.in.in \
	INSTALL \
	aclocal.m4 \
	config.h.in \
	config.h.in~ \
	configure \
	gtk-doc.make

clean-local:
	-debian/rules clean

distclean-local:
	rm -rf autom4te.cache build-aux m4 .anjuta .anjuta_sym_db.db

ChangeLog:

.PHONY: ChangeLog
