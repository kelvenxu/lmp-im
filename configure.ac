# Process this file with autoconf to produce a configure script.
AC_INIT([lmp-im], [1.6])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/lmp-im.c])
AM_CONFIG_HEADER([config.h])

AC_CANONICAL_HOST
case "$host" in
*-*-mingw*|*-*-cygwin*)
  platform_win32=yes
  ;;
*)
  platform_win32=no
  ;;
esac
AM_CONDITIONAL(PLATFORM_WIN32, test x"$platform_win32" = x"yes")

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK

AM_DISABLE_STATIC
AM_PROG_LIBTOOL

# Checks for libraries.
# AC_CHECK_LIB([rt], [clock_gettime])

# Checks for GTK+ libraries.
AM_PATH_GTK_2_0([2.0.0],,[AC_MSG_ERROR(GTK 2.0.0 or better is required.)])

GTK2_CFLAGS="`pkg-config --cflags gtk+-2.0`"
GTK2_LIBS="`pkg-config --libs gtk+-2.0`"
GTK2_BINARY_VERSION="`pkg-config --variable=gtk_binary_version gtk+-2.0`"
AC_SUBST([GTK2_CFLAGS])
AC_SUBST([GTK2_LIBS])
AC_SUBST([GTK2_BINARY_VERSION])

# I18N
#AM_GNU_GETTEXT_VERSION([0.16.1])
AM_GNU_GETTEXT([external])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h stdlib.h string.h wchar.h])

if test "x$ac_cv_header_stdc" = "xno"; then
  AC_MSG_ERROR([*** The required stdc header files not found.])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[[\ \	]]-Wall[[\ \	]]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
CFLAGS="$CFLAGS -D_GNU_SOURCE -D_REENTRANT -DDEBUG"

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([getline memchr memmove setlocale strchr strdup strrchr])
AC_CHECK_FUNCS([bind_textdomain_codeset], [break])
# AC_CHECK_FUNCS([clock_gettime], [break])

AC_CONFIG_FILES([Makefile
                 lpim.spec
                 config/Makefile
                 po/Makefile.in
                 src/Makefile
								 data/Makefile
                 ])
AC_OUTPUT
