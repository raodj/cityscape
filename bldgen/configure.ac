#   -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#---------------------------------------------------------------------------
#
# Copyright (c) Miami University, Oxford, OH.
# All rights reserved.
#
# This file is part of HAPLOS <http:#pc2lab.cec.miamiOH.edu/>
#
# Human  Population  and   Location  Simulator (HAPLOS)  is
# free software: you can  redistribute it and/or  modify it
# under the terms of the GNU  General Public License  (GPL)
# as published  by  the   Free  Software Foundation, either
# version 3 (GPL v3), or  (at your option) a later version.
#
# HAPLOS is distributed in the hope that it will  be useful,
# but   WITHOUT  ANY  WARRANTY;  without  even  the IMPLIED
# WARRANTY of  MERCHANTABILITY  or FITNESS FOR A PARTICULAR
# PURPOSE.
#
# Miami University and the HAPLOS  development team make no
# representations  or  warranties  about the suitability of
# the software,  either  express  or implied, including but
# not limited to the implied warranties of merchantability,
# fitness  for a  particular  purpose, or non-infringement.
# Miami  University and  its affiliates shall not be liable
# for any damages  suffered by the  licensee as a result of
# using, modifying,  or distributing  this software  or its
# derivatives.
#
# By using or  copying  this  Software,  Licensee  agree to
# abide  by the intellectual  property laws,  and all other
# applicable  laws of  the U.S.,  and the terms of the  GNU
# General  Public  License  (version 3).  You  should  have
# received a  copy of the  GNU General Public License along
# with HAPLOS.  If not, you may  download copies  of GPL V3
# from <http://www.gnu.org/licenses/>.
#
# Authors: Dhananjai M. Rao       raodm@muohio.edu
#
#---------------------------------------------------------------------------

AC_PREREQ(2.69)
AC_INIT([bldgen],[0.1.0],[raodm@muohio.edu])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_REQUIRE_CPP

# The macros above automatically fill in some CXXFLAGS so we'll undo it.
CFLAGS=""
CXXFLAGS=""
CPPFLAGS=""

# Check if mpicxx requires -lstdc++ option (needed for ICC redhawk)
AC_DEFUN([AC_LINK_STDCPP],
[
     AC_MSG_CHECKING([if $CXX needs stdc++])
     AC_LANG([C++])
     
     AC_LINK_IFELSE(
         [AC_LANG_PROGRAM([[#include <iostream>]],[[std::cout << "Hello";]])],
         [AC_MSG_RESULT([no])
	 ],
	 [AC_MSG_RESULT([yes])
	     LDFLAGS="-lstdc++ $LDFLAGS"
	 ])

])

# Check if we are using icc to supress a couple of useless remarks
AC_DEFUN([AC_PROG_ICC],
[
    AC_ARG_WITH([icc],
            AS_HELP_STRING([--with-icc],[Code will be compiled with icc]),
    [ if test "$withval" != "yes"; then
	  icc="yes"
      else
	  icc="no"
      fi
     ])

     if test -z "$icc"; then
         AC_MSG_CHECKING([for icc])
         icc=`mpicxx --version | grep -iw "icc"`
	 if test -n "$icc"; then
	     icc="yes"
	 else
	     icc="no"
	 fi
         AC_MSG_RESULT([$icc])
     fi

     if test "yes" == "$icc"; then
          USING_ICC="-DICC"
          ICC_NO_WARN_SOME="-wd981 -wd383 -wd424"
	  ICC_NO_WARN_FOR_CONFIGURE="-wd981 -wd383 -wd1419"
     else
          USING_ICC=""
          ICC_NO_WARN_SOME=""
	  ICC_NO_WARN_FOR_CONFIGURE=""
     fi

     AC_SUBST(USING_ICC)
     AC_SUBST(ICC_NO_WARN_SOME)
])

# A convenience macro to check if the compiler accepts a specific
# compiler flag or argument (and does not generate an error)
AC_DEFUN([AC_CHECK_ARGS],
[
    AC_MSG_CHECKING([whether _AC_LANG compiler accepts $1])
    if test -z "[$4]"; then
        AC_LANG_CONFTEST([AC_LANG_PROGRAM([[#include <iostream>]],
				          [[std::cout << "ac_check_args"]])])
        src=`ls conftest.c*`
        output=`$CXX $1 $CXXFLAGS $LIBS $ICC_NO_WARN_FOR_CONFIGURE $src 2>&1`
        if test -z "$output"; then
            AC_MSG_RESULT([yes])
            m4_default([$2], :)
	    CPPFLAGS+=" [$1]"
        else
	    AC_MSG_RESULT([no])
	    m4_default([$3], :)
        fi
    else
        AC_MSG_RESULT([skipped (because "$4" overrides)])
	m4_default([$3], :)
    fi
])

# Check if we are using correct version of gcc
AC_DEFUN([AC_CHECK_GCC_VERSION],
[
    AC_MSG_CHECKING([whether gcc is sufficiently new])
    # Get version from GCC version header in the form:
    # gcc (GCC) 4.x.x ...
    version=`g++ --version 2>&1 | head -1 | cut -d' ' -f3`
    majorVer=`echo $version | cut -d'.' -f1`
    minorVer=`echo $version | cut -d'.' -f2`
    patchLevel=`echo $version| cut -d'.' -f3`

    good="yes"
    # Check to ensure gcc is at least 4.0.1
    if test $majorVer -lt 4; then
       good="no"
    elif test $majorVer -eq 4; then
	 if test $minorVer -lt 8; then
 	     good="no"
	 fi
	     # Add checks for buggy gcc 4.0.1+ versions here
    fi

    if test $good = no; then
        AC_MSG_RESULT([no])
	AC_MSG_ERROR([
	  HAPLOS has been developed using the latest C++ standards.
	  It requires atleast GCC Version 4.9.2 to compile. The gcc
	  version on your machine is $version which is not
	  sufficiently recent to compile HAPLOS. You need upgrade
	  gcc. If you are on a cluster environment contact your
	  system adminstrator to determine how to load a more recent
	  gcc module by default.
	 ])
    else
        AC_MSG_RESULT([yes, $version])
    fi
])

# Check if we are using gcc to supress a couple of useless remarks
AC_DEFUN([AC_PROG_GCC],
[
    AC_ARG_WITH(gcc,
        AC_HELP_STRING([--with-gcc],[Code will be compiled with gcc]),
        [ if test "$withval" == "yes"; then
	  	 gcc="yes"
          else
		gcc="no"
          fi
        ])

     if test -z "$gcc"; then
         AC_MSG_CHECKING([for gcc])
         gcc=`$CXX --version 2>&1 | grep -i -w "GCC"`
	 if test -n "$gcc"; then
	     gcc="yes"
	 else
	     gcc="no"
	 fi
         AC_MSG_RESULT([$gcc])
     fi

     if test "yes" == "$gcc"; then
         USING_GCC="-DGCC"
	 AC_CHECK_GCC_VERSION
         AC_CHECK_ARGS([-O3], [OPT_O3="-O3"], [OPT_O3=""])
         AC_SUBST(OPT_O3)
     else
     	 USING_GCC=""
     fi

     AC_SUBST(USING_GCC) 
])

# Checks for programs.
AC_MSG_CHECKING([properties of C++ compiler])
AC_MSG_RESULT([$CXX])
AC_PROG_CXX
AC_LINK_STDCPP
AC_PROG_ICC
AC_PROG_GCC
AC_PROG_RANLIB

# Checks for suitable archiver to be used
AM_PROG_AR

# Check and set some default compiler flags for the build
AC_CHECK_ARGS([-Minform=warn], [INFORM_WARN="-Minform=warn"], [INFORM_WARN=""])
AC_SUBST(INFORM_WARN)
AC_CHECK_ARGS([-Wall], [WALL="-Wall"], [WALL=""], [])
AC_SUBST(WALL)
AC_CHECK_ARGS([-Wextra], [WEXTRA="-Wextra"], [WEXTRA=""], [])
AC_SUBST(WEXTRA)
AC_CHECK_ARGS([-pipe], [PIPE="-pipe"], [PIPE=""], [])
AC_SUBST(PIPE)
AC_CHECK_ARGS([-fast], [FAST="-fast"], [FAST=""], [])
AC_SUBST(FAST)

# Check and start using new C++14 standards
# AC_CHECK_ARGS([-std=c++14], [CPP14="-std=c++14"], [CPP14=""], [])
# AC_SUBST(CPP14)
AC_CHECK_ARGS([-std=c++11], [CPP11="-std=c++11"], [CPP11=""], [$CPP14])
AC_SUBST(CPP11)
AC_CHECK_ARGS([-std=c++0x], [CPP0X="-std=c++0x"], [CPP0X=""], [$CPP11])
AC_SUBST(CPP0X)

# Put object files in corresponding directories even if make is not
# running directly in the source directory.
AM_PROG_CC_C_O

# Checks for libraries.
AC_CHECK_LIB([m],[asin],,,[-lstdc++])
AC_SEARCH_LIBS([initscr],[curses],,,[-lstdc++])
AC_CHECK_LIB([readline],[readline],,,[-lstdc++])
AC_CHECK_LIB([pthread], [pthread_create],,,[-lstdc++])
AC_SEARCH_LIBS([gethostname], [c,nsl],,,[-lstdc++])
AC_SEARCH_LIBS([bind],[c,nsl,socket],,,[-lstdc++])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h malloc.h netdb.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/socket.h sys/time.h unistd.h values.h GL/glut.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_VOLATILE
AC_C_RESTRICT
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_FUNC_STRTOD
AC_CHECK_FUNCS([bzero floor gethostbyname gethostname gethrtime gettimeofday memset mkdir modf pow rint socket sqrt strcasecmp strchr strdup strstr strtol])

# Setup doxygen support
DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([BuildingGenerator], [docs/Doxyfile], [docs/html])

# Setup the configuration files to use
AC_CONFIG_FILES([Makefile\
	src/Makefile])

AC_OUTPUT

# End of script
