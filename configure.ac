#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([gensym], [1.0.0], [patrickhead@gmail.com])
AC_CONFIG_MACRO_DIR([.])
AC_CONFIG_AUX_DIR(tools)
AC_CONFIG_SRCDIR([src/gensym.c])
AC_CONFIG_HEADERS([src/config.h])

# Doxygen features and initialization

DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(gensym, doxygen.gensym.cfg)

AM_INIT_AUTOMAKE([subdir-objects])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([getcwd memset mkdir strcasecmp strdup strncasecmp strrchr])

# Checks for libraries
AC_CHECK_LIB([lihata], [lht_str_keyeq])
if test "x$ac_cv_lib_lihata_lht_str_keyeq" = xno; then
  AC_MSG_ERROR([liblihata is required])
fi
AC_CHECK_LIB([genht], [htss_alloc])
if test "x$ac_cv_lib_genht_htss_alloc" = xno; then
  AC_MSG_ERROR([libgenht is required])
fi
AC_CHECK_LIB([minuid], [minuid_init])
if test "x$ac_cv_lib_minuid_minuid_init" = xno; then
  AC_MSG_ERROR([libminuidihata is required])
fi



AC_CONFIG_FILES([Makefile])

AC_OUTPUT
