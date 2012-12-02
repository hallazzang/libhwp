ACLOCAL_AMFLAGS = -I m4
AM_DISTCHECK_CONFIGURE_FLAGS = --enable-introspection=no

SUBDIRS = \
	src \
	tests \
	$(NULL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ghwp-0.1.pc

dist-hook: gen-ChangeLog

gen_start_date = 2012-11-01
.PHONY: gen-ChangeLog
gen-ChangeLog:
	if test -d .git; then						\
	  $(top_srcdir)/gitlog-to-changelog				\
	    --since=$(gen_start_date) > $(distdir)/cl-t;		\
	  rm -f $(distdir)/ChangeLog;					\
	  mv $(distdir)/cl-t $(distdir)/ChangeLog;			\
	fi

EXTRA_DIST = \
	gitlog-to-changelog \
	ghwp-0.1.pc.in \
	$(NULL)

