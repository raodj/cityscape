# This file is processed by automake to generate Makefile.in

#---------------------------------------------------------------------------
#
#
# Copyright (c) Miami University, Oxford, OH.
# All rights reserved.
#
# Miami University (MU) makes no representations or warranties about
# the suitability of the software, either express or implied,
# including but not limited to the implied warranties of
# merchantability, fitness for a particular purpose, or
# non-infringement.  MU shall not be liable for any damages suffered
# by licensee as a result of using, result of using, modifying or
# distributing this software or its derivatives.
#
# By using or copying this Software, Licensee agrees to abide by the
# intellectual property laws, and all other applicable laws of the
# U.S., and the terms of this license.
#
# Authors: Dhananjai M. Rao       raodm@miamiOH.edu
#
#---------------------------------------------------------------------------

include $(top_srcdir)/Makefile.common.am

AM_CPPFLAGS+= -Wno-unused-local-typedefs
AM_CPPFLAGS+= $(BOOST_CPPFLAGS)

# first build pre-compiled header
BUILT_SOURCES = Boost.h.gch
CLEANFILES = Boost.h.gch

Boost.h.gch: Boost.h
	$(CXXCOMPILE) -MT $@ -MD -MP -MF ./$(DEPDIR)/Boost.h.Po -c -o $@ $<
	
# $(CXXCOMPILE) $(AM_CPPFLAGS) OsmSection.h

# Now use standard macros for building searums
bin_PROGRAMS = bldgen

bldgen_LDADD = $(BOOST_ASIO_LIB)
bldgen_LDFLAGS = $(BOOST_LDFLAGS)

bldgen_SOURCES = \
	Utils.h\
	Utils.cpp\
	OsmTags.h\
	OsmTags.cpp\
	OsmNode.h\
	OsmNode.cpp\
	OsmWay.h\
	OsmWay.cpp\
	OsmSection.h\
	OsmSection.cpp\
	OsmBuilding.h\
	OsmBuilding.cpp\
	BuildingGenerator.h\
	BuildingGenerator.cpp\
	OsmXFigHelper.h\
	OsmXFigHelper.cpp\
	XFigHelper.h\
	XFigHelper.cpp\
	ImageGenerator.h\
	ImageGenerator.cpp\
	ArgParser.h\
	ArgParser.cpp\
	Configuration.h\
	Configuration.cpp\
	main.cpp

# end of Makefile.am
