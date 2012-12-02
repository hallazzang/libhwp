AC_INIT([libghwp], [0.1], [https://github.com/cogniti/libghwp/issues], [libghwp])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([check-news dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

LIBGHWP_LT_VERSION="2:0:0"
AC_SUBST(LIBGHWP_LT_VERSION)

AM_PROG_VALAC([0.17.5])
AC_SUBST(VALAC)

AC_SUBST(VALAFLAGS)

GLIB_REQUIRED=2.12.0

PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED
                  libgsf-1 cairo gee-1.0)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

GOBJECT_INTROSPECTION_CHECK([0.9.0])

AC_OUTPUT([Makefile
           ghwp-0.1.pc
           src/Makefile
           tests/Makefile])
