# Process this file with autoconf to produce a configure script.

AC_INIT(lmp-im, 0.1.0, kelvenxu@gmail.com)
AC_CONFIG_SRCDIR([src/lmp-im.c])

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])


# Checks for programs.
AC_PROG_CC
AM_PROG_LIBTOOL

# Checks for GTK+ libraries.
#AM_PATH_GTK_2_0([2.0.0],,[AC_MSG_ERROR(GTK 2.0.0 or better is required.)])

PKG_CHECK_MODULES(GTK3, [gtk+-3.0 >= 3.0.0])
#PKG_CHECK_MODULES(GTK3, [gtk+-2.0 >= 2.0.0])

GTK3_BINARY_VERSION="`pkg-config --variable=gtk_binary_version gtk+-3.0`"
AC_SUBST([GTK3_CFLAGS])
AC_SUBST([GTK3_LIBS])
AC_SUBST([GTK3_BINARY_VERSION])

GETTEXT_PACKAGE=lmp-im
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [The gettext domain])

# Init gettext
ALL_LINGUAS="zh_CN"
AM_GNU_GETTEXT([external]) 


AC_CONFIG_FILES([Makefile 
                 lmp-im.spec
                 po/Makefile.in
                 src/Makefile
								 data/Makefile
                 ])
AC_OUTPUT

