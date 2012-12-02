noinst_PROGRAMS = $(TEST_PROGS)

TEST_PROGS = tests

tests_SOURCES = \
       test-ghwp.vala \
       $(NULL)

tests_DEPENDENCIES = \
	$(top_srcdir)/src/ghwp-0.1.vapi \
	$(NULL)

tests_VALAFLAGS = \
	--basedir $(top_srcdir) \
	--vapidir $(top_srcdir)/src \
	--pkg ghwp-0.1 --pkg libgsf-1 --pkg gee-1.0 --pkg cairo \
	$(VALAFLAGS) \
	$(NULL)

tests_CPPFLAGS = \
	-I$(top_srcdir)/src \
	$(GLIB_CFLAGS) \
	$(NULL)

tests_LDADD = \
	$(GLIB_LIBS) ../src/libghwp-0.1.la \
	$(NULL)
