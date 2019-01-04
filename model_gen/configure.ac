#-*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#---------------------------------------------------------------------------
#
# Copyright (c) PC2Lab Development Team
# All rights reserved.
#
# This file is part of free(dom) software -- you can redistribute it
# and/or modify it under the terms of the GNU General Public
# License (GPL)as published by the Free Software Foundation, either
# version 3 (GPL v3), or (at your option) a later version.
#
# The software is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# Miami University and PC2Lab makes no representations or warranties
# about the suitability of the software, either express or implied,
# including but not limited to the implied warranties of
# merchantability, fitness for a particular purpose, or
# non-infringement.  Miami University and PC2Lab is not be liable for
# any damages suffered by licensee as a result of using, result of
# using, modifying or distributing this software or its derivatives.
#
# By using or copying this Software, Licensee agrees to abide by the
# intellectual property laws, and all other applicable laws of the
# U.S., and the terms of this license.
#
# Authors: Dhananjai M. Rao       raodm@miamiOH.edu
#
#---------------------------------------------------------------------------

AC_PREREQ([2.69])
AC_INIT([modelgen], [0.1.0], [raodm@miamiOH.edu])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])

# Convenience macro to check for availability of GDAL
AC_DEFUN([AC_LIB_GDAL],
[
    AC_ARG_WITH(gdal,
            AC_HELP_STRING([--with-gdal=PATH],[path to GDAL library]),
    [ if test "$withval" != "yes"; then
          AC_MSG_CHECKING([for GDAL])
	  if test -x "$withval" ; then
	      GDAL="$withval"
	      AC_MSG_RESULT([$GDAL])
	  else
	      GDAL="no"
	      AC_MSG_RESULT(no)
	  fi
      fi
     ])
     
     if test -z "$GDAL"; then
        AC_LANG_PUSH([C++])
     	AC_CHECK_LIB(gdal, GDALAllRegister, [GDAL="yes"], [GDAL="no"])
        AC_LANG_POP([C++])
        if test "x$GDAL" == "xyes" ; then
            GDAL="/usr/"
        fi
     fi

     if test "$GDAL" = "no"; then
        AC_MSG_WARN([Could not find GDAL library. Some of the options in
                     the software will be disabled.  If you would like to
                     enable all the features, please install GDAL
                     libraries or ensure you have specified the correct
                     path to GDAL via --with-gdal option when running
                     configureto.])
     fi
     AC_SUBST(GDAL)
     AM_CONDITIONAL([GDAL], [test "x$GDAL" != "xno"])     
])

# Setup default programming language
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CXXCPP
AC_LANG([C++])

# Setup location of configure flag.
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_REQUIRE_CPP

# Checks for header files.
AC_CHECK_HEADERS([cmath iostream])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([sqrt atan2])

# Check for availability of gdal
AC_LIB_GDAL

# Check to see if hdf5 is available for optionally linking with it
AC_CHECK_LIB(hdf5_hl, H5open)
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
