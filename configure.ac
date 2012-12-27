AC_INIT([libghwp], [0.1.1], [https://github.com/cogniti/libghwp/issues], [libghwp])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC

LT_INIT

PKG_CHECK_MODULES(LIBGHWP, [libgsf-1 glib-2.0 gio-2.0 cairo gobject-2.0 cairo-ft freetype2 ])

dnl gsf_msole_metadata_read is deprecated since libgsf 1.14.24
dnl check if your libgsf-1 have gsf_doc_meta_data_read_from_msole
AC_CHECK_LIB(gsf-1, gsf_doc_meta_data_read_from_msole,
             AC_DEFINE(HAVE_GSF_DOC_META_DATA_READ_FROM_MSOLE,
                       [1],
                       [Define to 1 if your libgsf-1 have gsf_doc_meta_data_read_from_msole.]))

AC_OUTPUT([
Makefile
src/ghwp-0.1.pc
src/Makefile
])
