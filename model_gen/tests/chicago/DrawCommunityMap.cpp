#ifndef DRAW_COMMUNITY_MAP_CPP
#define DRAW_COMMUNITY_MAP_CPP

//---------------------------------------------------------------------------
//
// Copyright (c) PC2Lab Development Team
// All rights reserved.
//
// This file is part of free(dom) software -- you can redistribute it
// and/or modify it under the terms of the GNU General Public
// License (GPL)as published by the Free Software Foundation, either
// version 3 (GPL v3), or (at your option) a later version.
//
// The software is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Miami University and PC2Lab makes no representations or warranties
// about the suitability of the software, either express or implied,
// including but not limited to the implied warranties of
// merchantability, fitness for a particular purpose, or
// non-infringement.  Miami University and PC2Lab is not be liable for
// any damages suffered by licensee as a result of using, result of
// using, modifying or distributing this software or its derivatives.
//
// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.
//
// Authors: Dhananjai M. Rao       raodm@miamiOH.edu
//
//---------------------------------------------------------------------------

#include <unordered_map>
#include <sstream>
#include "ShapeFile.h"

/** The shape file containing rings loaded from community
    boundaries specified associated with the model.  For example,
    this object contains a set of rings loaded from
    boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.shp
    along with annotations loaded from
    boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.dbf
*/
ShapeFile communities;

// The various input files used in the main function below
const std::string shpFile  = "boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.shp";
const std::string dbfFile  = "boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.dbf";
const std::string infoFile = "taxi_ride_data/area_info_taxi_trips.tsv";

std::string getColumn(const std::string& line, const int column,
                      const char delim = '\t') {
    int currCol = 0, startPos = 0;
    while (currCol < column) {
        startPos = line.find(delim, startPos + 1);
        currCol++;
    }
    int nextTab = line.find(delim, startPos + 1);
    return line.substr(startPos, nextTab - startPos);
}

std::unordered_map<int, long> loadCommInfo(const int col = 2) {
    std::unordered_map<int, long> commInfo;
    std::ifstream info(infoFile);
    std::string line;
    // Discard header line
    std::getline(info, line);
    while (std::getline(info, line)) {
        const int areaID = std::stoi(getColumn(line, 0));
        const long info  = std::stol(getColumn(line, col));
        std::cout << areaID << ", " << info << std::endl;
        // Add entry to hash map
        commInfo[areaID] = info;
    }
    return commInfo;
}

int main(int argc, char *argv[]) {
    const int col = (argc > 1 ? std::stoi(argv[1]) : 2);
    // Load information for each community
    const std::unordered_map<int, long> commInfo = loadCommInfo(col);
    // Load the communities shape file.
    if (!communities.loadShapes(shpFile, dbfFile)) {
        std::cerr << "Error loading shape/DBF file: "
                  << shpFile << " or " << dbfFile << std::endl;
        return 2;
    }
    // Create a new shapefile with population ring for each community
    ShapeFile popRings;
    for (int i = 0; (i < communities.getRingCount()); i++) {
        // Get the source ring.
        const Ring& srcRing = communities.getRing(i);
        // Set the community information
        const int areaID = std::stoi(srcRing.getInfo("area_numbe"));
        // Create a new population ring using boundary info.
        Ring ring(srcRing);
        ring.setPopulation(commInfo.at(areaID));
        ring.setKind(Ring::POPULATION_RING);
        // Add ring to new shape file for plotting
        popRings.addRing(ring);
    }
    // Finally draw the communities with color coding
    popRings.genXFig("community_info.fig", 1638400);
    // All done
    return 0;
}

#endif
