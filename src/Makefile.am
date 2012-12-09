lib_LTLIBRARIES = \
	libghwp-0.1.la
	$(NULL)

libghwp_0_1_la_SOURCES = \
	tags.vala \
	ghwp-file.vala \
	document.vala \
	parse.vala \
	$(NULL)

libghwp_0_1_la_VALAFLAGS = \
	-H ghwp.h --vapi ghwp-0.1.vapi \
	--library ghwp-0.1 --gir GHWP-0.1.gir \
	--pkg libgsf-1 --pkg gee-1.0 --pkg cairo \
	$(VALAFLAGS) \
	$(NULL)

libghwp_0_1_la_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(NULL)

libghwp_0_1_la_LIBADD = \
	$(GLIB_LIBS) \
	$(NULL)

libghwp_0_1_la_LDFLAGS = \
	-version-info $(LIBGHWP_LT_VERSION) \
	-no-undefined \
	$(NULL)

ghwpincludedir = $(includedir)/ghwp-0.1

ghwpinclude_HEADERS = \
	ghwp.h \
	$(NULL)

vapidir = $(datadir)/vala/vapi

dist_vapi_DATA = \
	ghwp-0.1.vapi \
	$(NULL)

if HAVE_INTROSPECTION
girdir = @INTROSPECTION_GIRDIR@

gir_DATA = \
	GHWP-0.1.gir \
	$(NULL)

typelibdir = @INTROSPECTION_TYPELIBDIR@
typelib_DATA = \
	GHWP-0.1.typelib \
	$(NULL)

# Extract dlname from libfolks.la;
# See bug https://bugzilla.gnome.org/show_bug.cgi?id=658002
# This is what g-ir-scanner does.
libghwp_dlname = \
	`$(SED) -nE "s/^dlname='([A-Za-z0-9.+-]+)'/\1/p" libghwp-0.1.la`
INTROSPECTION_COMPILER_ARGS= \
	-l $(libghwp_dlname)
GHWP-0.1.typelib: GHWP-0.1.gir
	@INTROSPECTION_COMPILER@ $(INTROSPECTION_COMPILER_ARGS) -o $@ $^
endif

GHWP-0.1.gir ghwp-0.1.vapi: libghwp-0.1.la

CLEANFILES = \
	ghwp-0.1.vapi \
	GHWP-0.1.typelib \
	GHWP-0.1.gir \
	$(NULL)

