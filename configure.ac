#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([dbus_echo], [0.1], [nirmoy.hcu@gmail.com])
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([dbus_echo.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
DBUS_REQUIRED=1.3.1
DBUS_GLIB_REQUIRED=0.82
GLIB_REQUIRED=2.26.0

PKG_CHECK_MODULES(DBUS, [dbus-1 >= $DBUS_REQUIRED ])
AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)
# Checks for library functions.
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
