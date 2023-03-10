
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

bin_PROGRAMS = cab_sim

AM_CXXFLAGS  = -g -Wall -std=c++11 -DNO_XFIG -I$(MUSE)/include

# For profiling the frame pointers are important for short methods.
AM_CXXFLAGS += -fno-omit-frame-pointer

# Developer assertions include sanity-checks (but degrade performance slightly)
AM_CXXFLAGS += -DDEVELOPER_ASSERTIONS

# Link-time information for cab_sim
cab_sim_LDFLAGS = -L$(MUSE)/kernel
cab_sim_LDADD = -lmuse -lpthread -lnuma
cab_sim_DEPENDENCIES= $(MUSE)/kernel/libmuse.a

cab_sim_SOURCES = \
	Utilities.h \
	Utilities.cpp \
	Ring.h \
	Ring.cpp \
	PopRing.h \
	PopRing.cpp \
	OSMData.h \
	OSMData.cpp \
	Node.h \
	Node.cpp \
	Building.h \
	Building.cpp \
	Way.h \
	Way.cpp \
	PathSegment.h \
	PathSegment.cpp \
	IndexedPriorityQueue.h \
	PathFinder.h \
	PathFinder.cpp \
	XFigHelper.h \
	XFigHelper.cpp \
	PopGrid.h \
	PopGrid.cpp \
	SimConfig.h \
	SimConfig.cpp \
	Area.h \
	Area.cpp \
	CabSim.h \
	CabSim.cpp \
	main.cpp

# End of Makefile.am
