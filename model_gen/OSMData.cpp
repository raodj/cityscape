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
#include <limits>
#include "OSMData.h"
#include "PathSegment.h"
#include "Utilities.h"

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
    PersonMap personMap;   // Cross-reference for person entries
    std::string line;
    while (std::getline(model, line)) {
        if (line.empty() || line.front() == '#') {
            if (line.substr(0, 5) == "# Per ") {
                // Load column titles for person entries
                PUMSPerson::readColTitles(line);
            }
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
        } else if (line.substr(0, 3) == "hld") {
            PUMSHousehold hld;
            hld.read(is, personMap);
            // Add hosuehold to its corresponding building.
            if (buildingMap.find(hld.getBuildingID()) != buildingMap.end()) {
                // Add this household to its building.
                Building& bld = buildingMap.at(hld.getBuildingID());
                bld.addHousehold(hld, hld.getPeopleCount(),
                                 hld.getPeopleInfo());
            }
        } else if (line.substr(0, 3) == "per") {
            PUMSPerson per;
            per.read(is);
            ASSERT(personMap.find(per.getPerID()) == personMap.end());
            personMap[per.getPerID()] = per;
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

void
OSMData::saveModel(const std::string& filePath,
                   const std::string& cmdLine) const {
    if (filePath.empty()) {
        return;
    }
    std::ofstream out(filePath);
    if (!out.good()) {
        throw std::runtime_error("Error writing model file: " + filePath);
    }
    out << std::boolalpha;
    // Header comments
    out << "# Model updated by schedule_generator\n"
        << "# Command-line used: " << cmdLine << "\n"
        << "# Rings in model: "    << popRings.size()    << "\n"
        << "# Nodes in model: "    << nodeList.size()    << "\n"
        << "# Buildings in model: "<< buildingMap.size() << "\n"
        << "# Ways in model: "     << wayMap.size()      << "\n\n";

    // Population rings
    for (size_t i = 0; i < popRings.size(); i++) {
        const PopRing& pr = popRings[i];
        if (i == 0) {
            out << "# Rng ID ringID shpaeID pop num_vert tl_lat tl_lon"
                   " br_lat br_lon num_bld bld_sqFt homes home_sqFt info\n";
        }
        const std::streamsize prec =
            out.precision(std::numeric_limits<double>::digits10 + 1);
        out << "rng " << pr.id     << " " << pr.ringID  << " " << pr.shapeID
            << " "    << pr.getPopulation() << " " << pr.getVertexCount()
            << " "    << pr.tlLat  << " "  << pr.tlLon
            << " "    << pr.brLat  << " "  << pr.brLon
            << " "    << pr.numBuildings   << " " << pr.bldsTotSqFt
            << " "    << pr.numHomes       << " " << pr.homesTotSqFt
            << pr.info << "\n";
        out.precision(prec);
    }
    out << "\n";

    // Nodes (already in index order from loadModel)
    for (size_t i = 0; i < nodeList.size(); i++) {
        nodeList[i].write(out, i == 0);
    }

    // Ways
    out << "\n# Ways in model: " << wayMap.size() << "\n";
    bool firstWay = true;
    for (const auto& entry : wayMap) {
        entry.second.write(out, firstWay);
        firstWay = false;
    }

    // Buildings
    out << "\n# Buildings in model: " << buildingMap.size() << "\n";
    bool firstBld = true;
    for (const auto& entry : buildingMap) {
        entry.second.write(out, firstBld);
        firstBld = false;
    }

    // Persons
    bool writeHeader = true;
    for (const auto& entry : buildingMap) {
        for (const PUMSHousehold& hld : entry.second.households) {
            for (const auto& per : hld.getPeopleInfo()) {
                per.write(out, writeHeader);
                writeHeader = false;
            }
        }
    }

    // Households
    writeHeader = true;
    for (const auto& entry : buildingMap) {
        if (!entry.second.households.empty()) {
            entry.second.writeHouseholds(out, writeHeader);
            writeHeader = false;
        }
    }
    std::cout << "Model saved to: " << filePath << "\n";
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
