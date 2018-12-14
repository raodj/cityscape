
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

bin_PROGRAMS = modelgen pathfinder

AM_CXXFLAGS = -g -Wall -std=c++14 -DDEVELOPER_ASSERTIONS -fopenmp

modelgen_LDFLAGS = 
modelgen_LDADD = -lhdf5_hl -lgdal
modelgen_DEPENDENCIES=

modelgen_SOURCES = \
	shapefil.h \
	shpopen.c \
	shptree.c \
	dbfopen.c \
	ArgParser.cpp \
	ArgParser.h \
	Ring.cpp \
	Ring.h \
	ShapeFile.cpp \
	ShapeFile.h \
	Utilities.cpp \
	Utilities.h \
	XFigHelper.cpp \
	XFigHelper.h \
	PopulationRingGenerator.h \
	PopulationRingGenerator.cpp \
	ModelGenerator.h \
	ModelGenerator.cpp \
	Building.h \
	Building.cpp \
	Node.h \
	Node.cpp \
	Way.h \
	Way.cpp \
	main.cpp

pathfinder_SOURCES = \
	ArgParser.cpp \
	ArgParser.h \
	Utilities.cpp \
	Utilities.h \
	XFigHelper.cpp \
	XFigHelper.h \
	Ring.h \
	Ring.cpp \
	Building.h \
	Building.cpp \
	Node.h \
	Node.cpp \
	Way.h \
	Way.cpp \
	PathSegment.h \
	PathSegment.cpp \
	PathFinder.h \
	PathFinder.cpp

# End of Makefile.am
