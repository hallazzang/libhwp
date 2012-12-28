## Process this file with automake to produce Makefile.in

SUBDIRS = src 

libghwpdocdir = ${prefix}/share/doc/libghwp
libghwpdoc_DATA = \
	README    \
	COPYING   \
	AUTHORS   \
	ChangeLog \
	INSTALL   \
	NEWS      \
	THANKS


EXTRA_DIST = $(libghwpdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(libghwpdocdir)
