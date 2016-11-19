#ifndef CONFIGURATION_CPP
#define CONFIGURATION_CPP

//------------------------------------------------------------
//
// This file is part of HAPLOS <http://pc2lab.cec.miamiOH.edu/>
//
// Human  Population  and   Location  Simulator (HAPLOS)  is
// free software: you can  redistribute it and/or  modify it
// under the terms of the GNU  General Public License  (GPL)
// as published  by  the   Free  Software Foundation, either
// version 3 (GPL v3), or  (at your option) a later version.
//
// HAPLOS is distributed in the hope that it will  be useful,
// but   WITHOUT  ANY  WARRANTY;  without  even  the IMPLIED
// WARRANTY of  MERCHANTABILITY  or FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Miami University and the HAPLOS  development team make no
// representations  or  warranties  about the suitability of
// the software,  either  express  or implied, including but
// not limited to the implied warranties of merchantability,
// fitness  for a  particular  purpose, or non-infringement.
// Miami  University and  its affiliates shall not be liable
// for any damages  suffered by the  licensee as a result of
// using, modifying,  or distributing  this software  or its
// derivatives.
//
// By using or  copying  this  Software,  Licensee  agree to
// abide  by the intellectual  property laws,  and all other
// applicable  laws of  the U.S.,  and the terms of the  GNU
// General  Public  License  (version 3).  You  should  have
// received a  copy of the  GNU General Public License along
// with HAPLOS.  If not, you may  download copies  of GPL V3
// from <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------

#include "Configuration.h"
#include "ArgParser.h"

Configuration::Configuration() {
    printWays     = false;
    zoomLevel     = 15;
    drawWays      = true;
    drawBuildings = true;
}

Configuration::~Configuration() {
    // Nothing to be done here.
}

bool
Configuration::process(int& argc, char *argv[]) {
    // Make the arg_record to parse arguments.
    ArgParser::ArgRecord arg_list[] = {
        {"--osm-xml", "-in", "Path to input OSM XML file to be used.",
         &osmXmlPath, ArgParser::STRING},
        {"--print-ways", "", "Print ways (and nodes) loaded from XML",
         &printWays, ArgParser::BOOLEAN},
        {"--fig-file", "-fig", "Path to output figure file",
         &xfigPath, ArgParser::STRING},
        {"--zoom-level", "-zl", "Zoom level (8 to 18) for output figure",
         &zoomLevel, ArgParser::INTEGER},
        {"--draw-ways", "", "Draw the ways in OSM XML for verification",
         &drawWays, ArgParser::BOOLEAN},
        {"--draw-buildings", "-db", "Draw the buildings for verification",
         &drawBuildings, ArgParser::BOOLEAN},
        {"--cache", "", "Cache directory to use for map tile images",
         &cacheDir, ArgParser::STRING},
        {"", "", "", NULL, ArgParser::INVALID}
    };
    // Parse the command-line arguments & set instance variables.
    ArgParser ap(arg_list);
    ap.parseArguments(argc, argv, true);
    // Check to ensure we have an OSM file to process
    if (osmXmlPath.empty()) {
        std::cerr << "You need to specify a OSM XML file to process.\n";
        std::cerr << ap << std::endl;
        return false;
    }
    // Things look good so far.
    return true;
}

#endif
