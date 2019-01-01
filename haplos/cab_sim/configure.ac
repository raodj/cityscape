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

# Setup default programming language
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CXXCPP
AC_LANG([C++])

# Set that we are using mpicxx as our compiler
CXX=mpicxx

# Setup location of configure flag.
AC_CONFIG_SRCDIR([src/config.h.in])
AC_CONFIG_HEADERS([src/config.h])
AC_REQUIRE_CPP

AC_DEFUN([AC_LIB_MUSE],
[
    AC_ARG_WITH(muse,
            AC_HELP_STRING([--with-muse=PATH],[path to muse]),
    [ if test "$withval" != "yes"; then
          AC_MSG_CHECKING([for MUSE])
          if test -x "$withval" ; then
              MUSE="$withval"
              AC_MSG_RESULT([$MUSE])
          else
              MUSE="no"
              AC_MSG_RESULT(no)
          fi
      fi
     ])
     
     if test -z "$MUSE"; then
        AC_LANG_PUSH([C++])
        AC_CHECK_LIB(muse, muse::Simulation::getSimulator, [MUSE="yes"],
                     [MUSE="no"])
        AC_LANG_POP([C++])                     
     fi

     if test "$MUSE" = "no"; then
        AC_MSG_ERROR([MUSE is required to compile cab_sim.
                      Please obtain a copy of MUSE to continue.
                      Please check that MUSE is installed on your system
                      and your path includes the top-level install directory
                      (example: /usr/include/muse), or specify
                      --with-muse=PATH option when running configure.])
     fi
     AC_SUBST(MUSE)
])


# Checks for header files.
AC_CHECK_HEADERS([cmath iostream])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([sqrt atan2])

# Check to ensure that MUSE PDES kernel is available
AC_LIB_MUSE

# Generate the makefile
AC_CONFIG_FILES([Makefile
        src/Makefile])

AC_OUTPUT
