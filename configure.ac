#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(CoordinateTransform, 1.0.0.0, xuetaomagicsong@gmail.com)
AM_INIT_AUTOMAKE(CoordinateTransform,1.0.0.0)
AC_CONFIG_SRCDIR([CoordinateTransformer+.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AX_CXX_COMPILE_STDCXX
CXXFLAGS='-Wall -std=gnu++0x'
# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([sqrt])

AC_CONFIG_FILES([makefile])
AC_OUTPUT
