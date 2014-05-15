## Process this file with automake to produce Makefile.in

SUBDIRS = src docs po

libhwpdocdir = ${prefix}/share/doc/libhwp
libhwpdoc_DATA =  \
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

MAINTAINERCLEANFILES =         \
	$(srcdir)/ChangeLog    \
	$(srcdir)/INSTALL      \
	$(srcdir)/aclocal.m4   \
	$(srcdir)/compile      \
	$(srcdir)/config.guess \
	$(srcdir)/config.h.in  \
	$(srcdir)/config.h.in~ \
	$(srcdir)/config.sub   \
	$(srcdir)/configure    \
	$(srcdir)/depcomp      \
	$(srcdir)/gtk-doc.make \
	$(srcdir)/install-sh   \
	$(srcdir)/ltmain.sh    \
	$(srcdir)/missing      \
	`find "$(srcdir)" -type f -name Makefile.in -print` \
	-r $(srcdir)/m4

ChangeLog:
	$(AM_V_GEN) if test -d "$(srcdir)/.git"; then \
	  (GIT_DIR=$(top_srcdir)/.git ./missing --run git log --stat) | fmt --split-only > $@.tmp \
	  && mv -f $@.tmp $@ \
	  || ($(RM) $@.tmp; \
	      echo Failed to generate ChangeLog, your ChangeLog may be outdated >&2; \
	      (test -f $@ || echo git-log is required to generate this file >> $@)); \
	else \
	  test -f $@ || \
	  (echo A git checkout and git-log is required to generate ChangeLog >&2 && \
	  echo A git checkout and git-log is required to generate this file >> $@); \
	fi

.PHONY: ChangeLog
