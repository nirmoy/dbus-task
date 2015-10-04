#AM_CFLAGS= @DBUS_CFLAGS@ -Wall -g -DDBG
AM_CFLAGS= @DBUS_CFLAGS@ -Wall -g 
AM_LDFLAGS= @DBUS_LIBS@
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4 

bin_PROGRAMS = dbus_echo
dbus_echo_SOURCES = dbus_echo.c logger.c logger.h


lib_LTLIBRARIES = libpam_ignore.la
libpam_ignore_la_SOURCES = test_pam.c logger.c
libpam_ignore_la_LIBADD = @DBUS_LIBS@ 
libpam_ignore_la_LDFLAGS = @DBUS_LIBS@ 
libpam_ignore_la_CFLAGS = @DBUS_CFLAGS@ 
EXTRA_DIST = test.conf autogen.sh
