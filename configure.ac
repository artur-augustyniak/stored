#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# AC_PREREQ([2.69])
AC_INIT([stored], [0.1], [artur@aaugustyniak.pl])
AC_CONFIG_SRCDIR([src/stored.c])
AC_CONFIG_HEADERS([./config/config.h])
AH_BOTTOM([#include <custom_config.h>])
AC_CONFIG_AUX_DIR([./config])
AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE([disable])

# Checks for programs.
: ${CFLAGS=""}
AC_PROG_CC

PKG_PROG_PKG_CONFIG
AC_ARG_WITH([systemdsystemunitdir],
        AS_HELP_STRING([--with-systemdsystemunitdir=DIR], [Directory for systemd service files]),
        [], [with_systemdsystemunitdir=$($PKG_CONFIG --variable=systemdsystemunitdir systemd)])
AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])
AM_CONDITIONAL(HAVE_SYSTEMD, [test -n "$with_systemdsystemunitdir"])

# Checks for libraries.

# Checks for header files.
# AC_CHECK_HEADERS([fcntl.h limits.h stdint.h stdlib.h string.h sys/statvfs.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
# AC_TYPE_PID_T
# AC_TYPE_UINT64_T

# Checks for library functions.
# AC_FUNC_FORK
# AC_CHECK_FUNCS([memset strerror])

#AC_OUTPUT
AC_OUTPUT(Makefile src/Makefile doc/Makefile examples/Makefile man/Makefile scripts/Makefile systemd/Makefile)
