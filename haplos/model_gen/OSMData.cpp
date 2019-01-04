#ifndef OSM_DATA_CPP
#define OSM_DATA_CPP

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

#include <fstream>
#include <sstream>
#include <algorithm>
#include "OSMData.h"
#include "PathSegment.h"

int
OSMData::loadModel(const std::string& modelFilePath) {
    // Ensure model file is readable.
    std::ifstream model(modelFilePath);
    if (!model.good()) {
        std::cerr << "Error opening model file " << modelFilePath << std::endl;
        return 1;
    }
    // Process line-by-line from the model text file to load model
    // data into memory.
    std::string line;
    while (std::getline(model, line)) {
        if (line.empty() || line.front() == '#') {
            continue;  // empty or comment line. Ignore.
        }
        // Wrap string in a stream to ease extractions in if-else below
        std::istringstream is(line);
        // Read boolean as "true"/"false" and not 1/0
        is >> std::boolalpha;
        // Create objects based on type of input
        if (line.substr(0, 4) == "node") {
            Node node;
            node.read(is);
            nodeList.push_back(node);     // Add node to list
            nodesWaysList.push_back({});  // Add empty list for now
        } else if (line.substr(0, 3) == "way") {
            Way way;
            way.read(is);
            wayMap[way.id] = way;
        } else if (line.substr(0, 3) == "bld") {
            Building bld;
            bld.read(is);
            buildingMap[bld.id] = bld;
        } else if (line.substr(0, 3) == "rng") {
            PopRing rng;
            rng.read(is);
            popRings.push_back(rng);
        } else {
            std::cerr << "Invalid line in model file: " << line << std::endl;
            return 1;
        }
    }
    // Update the cross reference between nodes and ways.
    computeNodesWaysList();
    // Print aggregate information for cross checking.
    std::cout << "Loaded "     << nodeList.size() << " nodes, "
              << wayMap.size() << " ways, and "   << buildingMap.size()
              << " buildings.\n";
    // Everything went well
    return 0;
}

// Build look-up map to find the ways that intersect at a given node
void
OSMData::computeNodesWaysList() {
    for (const auto& entry : wayMap) {
        const long wayID = entry.second.id;
        for (const long nodeID : entry.second.nodeList) {
            nodesWaysList.at(nodeID).push_back(wayID);
        }
    }
}

// Convenience stream-insertion operator for OSMData::Info
std::ostream& operator<<(std::ostream& os, const OSMData::Info& info) {
    return (os << info.first << ":" << info.second << "(" << info.third << ")");
}

OSMData::InfoVec
OSMData::getSortedPopRingInfo(int infoKind, long& total) const {
    InfoVec infoList;  // The list of information to be returned.
    infoList.reserve(popRings.size());
    // First build infoList based on the attribute requested.
    total = 0;
    for (const PopRing& pr : popRings) {
        // Get the attribute of interest.
        const long info = (infoKind == 0 ? pr.getPopulation() :
                           (infoKind == 1 ? pr.bldsTotSqFt : pr.homesTotSqFt));
        // Add it to our list
        infoList.push_back(Info{pr.id, info});
        total += info;
    }
    // Finally sort the list
    std::sort(infoList.begin(), infoList.end());
    // Return the sorted list of information back to the caller
    return infoList;
}

OSMData::InfoVec
OSMData::getSortedBldInfo(int ringID, int infoKind, long& total) const {
    InfoVec infoList;  // The list of information to be returned.
    // First build infoList based on the attribute requested.
    total = 0;
    for (auto entry : buildingMap) {
        if ((ringID != -1) && (entry.second.attributes != ringID)) {
            continue;  // Building not in ring.
        }
        // Get building attribute of interest
        const Building& bld = entry.second;        
        const long info = (infoKind == 0 ? bld.population :
                           bld.sqFootage * std::max(1, bld.levels));
        // Add it to our list
        infoList.push_back(Info{bld.id, info, bld.attributes});
        total += info;
    }
    // Finally sort the list
    std::sort(infoList.begin(), infoList.end());
    // Return the sorted list of information back to the caller
    return infoList;
    
}

#endif
