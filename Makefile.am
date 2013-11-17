## Process this file with automake to produce Makefile.in

SUBDIRS = src 

libhwpdocdir = ${prefix}/share/doc/libhwp
libhwpdoc_DATA = \
	README    \
	COPYING   \
	AUTHORS   \
	ChangeLog \
	INSTALL   \
	NEWS      \
	THANKS


EXTRA_DIST = $(libhwpdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(libhwpdocdir)
