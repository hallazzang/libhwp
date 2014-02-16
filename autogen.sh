#!/bin/sh
# Run this to generate all the initial makefiles, etc.

test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

olddir=`pwd`
cd "$srcdir"

GTKDOCIZE=`which gtkdocize`
if test -z $GTKDOCIZE; then
        echo "*** No GTK-Doc found, please install it ***"
        exit 1
else
        gtkdocize || exit $?
fi

AUTORECONF=`which autoreconf`
if test -z $AUTORECONF; then
  echo "*** No autoreconf found, please install it ***"
  exit 1
fi

autoreconf --force --install --verbose || exit $?

if (grep "^IT_PROG_INTLTOOL" $srcdir/configure.ac >/dev/null); then
  INTLTOOLIZE=`which intltoolize`
  if test -z $INTLTOOLIZE; then
    echo "*** No intltoolize found, please install it ***"
    exit 1
  fi
  intltoolize --copy --force --automake
fi

cd "$olddir"
test -n "$NOCONFIGURE" || "$srcdir/configure" "$@"
