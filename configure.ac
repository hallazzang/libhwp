# *****************************************************************************
# Versioning
# *****************************************************************************

m4_define([hwp_major_version],[0])
m4_define([hwp_minor_version],[0])
m4_define([hwp_micro_version],[4])
m4_define([hwp_version],[hwp_major_version.hwp_minor_version.hwp_micro_version])

# Libtool versioning. The backend and view libraries have separate versions.
# Before making a release, the libtool version should be modified.
# The string is of the form C:R:A.
# - If interfaces have been changed or added, but binary compatibility has
#   been preserved, change to C+1:0:A+1
# - If binary compatibility has been broken (eg removed or changed interfaces)
#   change to C+1:0:0
# - If the interface is the same as the previous version, change to C:R+1:A

# Libtool version
m4_define([hwp_lt_current], [3])
m4_define([hwp_lt_revision],[0])
m4_define([hwp_lt_age],     [0])
m4_define([hwp_lt_version_info],[hwp_lt_current:hwp_lt_revision:hwp_lt_age])
m4_define([hwp_lt_current_minus_age],[m4_eval(hwp_lt_current - hwp_lt_age)])

# *****************************************************************************

AC_INIT([libhwp], [hwp_version],
        [https://bitbucket.org/hodong/libhwp/issues], [libhwp],
        [https://bitbucket.org/hodong/libhwp])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wno-portability])

AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_C_O

LT_INIT

dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=libhwp
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT

PKG_CHECK_MODULES(HWP_DEP,
                  [libgsf-1 cairo pango pangocairo libxml-2.0
                   gobject-2.0 gobject-introspection-1.0 poppler-glib])
AC_SUBST(HWP_DEP_CFLAGS)
AC_SUBST(HWP_DEP_LIBS)

GOBJECT_INTROSPECTION_CHECK([0.10.1])

dnl gsf_msole_metadata_read is deprecated since libgsf 1.14.24
dnl check if your libgsf-1 have gsf_doc_meta_data_read_from_msole
AC_CHECK_LIB(gsf-1, gsf_doc_meta_data_read_from_msole,
             AC_DEFINE(HAVE_GSF_DOC_META_DATA_READ_FROM_MSOLE,
                       [1],
                       [Define to 1 if your libgsf-1 have gsf_doc_meta_data_read_from_msole.]))

# **********
# Versioning
# **********

AC_SUBST([HWP_MAJOR_VERSION],[hwp_major_version])
AC_SUBST([HWP_MINOR_VERSION],[hwp_minor_version])
AC_SUBST([HWP_MICRO_VERSION],[hwp_micro_version])

AC_SUBST([HWP_LT_VERSION_INFO],[hwp_lt_version_info])
AC_SUBST([HWP_LT_CURRENT_MINUS_AGE],[hwp_lt_current_minus_age])

# **********************************

# *****************
# check for gtk-doc
# *****************
GTK_DOC_CHECK([1.14],[--flavour no-tmpl])
AM_CONDITIONAL([ENABLE_GTK_DOC], true)

# ****************************************

# *********
# Debugging
# *********

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [turn on debugging])],
  [enable_debug=yes],
  [enable_debug=no]
)

if test "x$enable_debug" = "xyes"; then
   HWP_DEBUG_FLAGS="-DHWP_ENABLE_DEBUG"
fi

AC_SUBST(HWP_DEBUG_FLAGS)

# **********************************

AC_OUTPUT([
    Makefile
    docs/Makefile
    docs/version.xml
    po/Makefile.in
    src/hwp-0.0.4.pc
    src/Makefile
    src/hwp-version.h
])
