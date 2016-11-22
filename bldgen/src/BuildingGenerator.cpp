#ifndef BUILDING_GENERATOR_CPP
#define BUILDING_GENERATOR_CPP

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

#include "Boost.h"
#include "BuildingGenerator.h"
#include "ImageGenerator.h"

BuildingGenerator::BuildingGenerator() {
    // Nothing else to be done here.
}

BuildingGenerator::~BuildingGenerator() {
    // Nothing else to be done here.
}

void
BuildingGenerator::process(int& argc, char *argv[]) {
    if (!config.process(argc, argv)) {
        return;  // insufficient command-line args.
    }
    // Load the OSM XML file into the osm object (throws exception)
    osm.load(config.osmXmlPath);
    // If asked, print the information in the osm class.
    if (config.printWays) {
        osm.printWays();
    }
    // Generate buildings using helper method
    generate();
    // Generate fig output if fig file is set.
    if (!config.xfigPath.empty()) {
        ImageGenerator ig(config.zoomLevel, config.cacheDir);
        ig.generate(*this, config.xfigPath, config.drawWays,
                    config.drawBuildings);
    }
}

void
BuildingGenerator::generate() {
    // Process each way in the OSM section.
    for (const OsmWay& way : osm.getWays()) {
        // If the tag on the way for 'highway' attribute is
        // residential then generate homes on this way.
        if (way["highway"] == "residential") {
            generateHomes(way);
        }
    }
}

void
BuildingGenerator::generateHomes(const OsmWay& way) {
    // way.print(osm.getNodes(), std::cout);

    // Currently this building generator does something rather simple.
    // It creates a home mid-way on each segment in the given way.
    const NodeList& nodeIDs = way.getNodeIDs();
    // Create 1 home mid-way on each segment.
    for (size_t i = 1; (i < nodeIDs.size()); i++) {
        // Get the node objects for the 2 consecutive node IDs
        const Location prev = osm.getNode(nodeIDs[i - 1]).getLocation();
        const Location curr = osm.getNode(nodeIDs[i]).getLocation();
        // Compute the mid-way latitude and longitude
        const double midLat = (prev.first  + curr.first)  / 2.0;
        const double midLon = (prev.second + curr.second) / 2.0;
        // Create a home at the midLat, midLon.
        const long id = bldList.size();
        OsmBuilding home(OsmBuilding::HOME, id, midLat, midLon, 0.009);
        // Add newly created home to the list of buildings
        bldList.push_back(home);
    }
}

double
BuildingGenerator::getLength(const OsmWay& way) const {
    const NodeList& nodeIDs = way.getNodeIDs();
    double distance = 0;
    for (size_t i = 1; (i < nodeIDs.size()); i++) {
        // Get the node objects for the 2 consecutive node IDs
        const OsmNode& prev = osm.getNode(nodeIDs[i - 1]);
        const OsmNode& curr = osm.getNode(nodeIDs[i]);
        // Compute distance between nodes and track total
        distance += (curr - prev);
    }
    // Return total length of the way (in miles)
    return distance;
}

#endif
