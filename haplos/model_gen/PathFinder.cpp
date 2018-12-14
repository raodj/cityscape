#ifndef PATH_FINDER_CPP
#define PATH_FINDER_CPP

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

#include <sstream>
#include "PathSegment.h"
#include "PathFinder.h"
#include "Utilities.h"

int
PathFinder::run(int argc, char *argv[]) {
    int error = 0;  // Error from various helper methods.
    // First process the command-line args and ensure we have
    // necessary arguments for performing various operations.
    if ((error = processArgs(argc, argv)) != 0) {
        return error;  // Error processing command-line args.
    }
    // Next load the community shape file
    if ((error = loadModel(cmdLineArgs.modelFilePath)) != 0) {
        return error;  // Error loading community shape file.
    }
    // Ensure starting and ending building ID's are valid.
    if ((buildingMap.find(cmdLineArgs.startBldID) == buildingMap.end()) ||
        (buildingMap.find(cmdLineArgs.endBldID)   == buildingMap.end())) {
        std::cerr << "Starting or ending building not found in model.\n";
        return 3;
    }
    // Get the building information for quick reference.
    const Building begBld = buildingMap.at(cmdLineArgs.startBldID);
    const Building endBld = buildingMap.at(cmdLineArgs.endBldID);    
    // Setup the beginning and destination path segmenets for further
    // processing.
    PathSegment begSeg{begBld.wayID, -1, begBld.id, 0};
    PathSegment endSeg{endBld.wayID, -1, endBld.id, 0};
    // Print the resulting path segement between the two
    std::cout << getPathOnSameWay(begSeg, endSeg) << std::endl;
    // Everything went well
    return 0;
}

Point
PathFinder::getLatLon(const PathSegment& path) const {
    // Return value from the node as the reference.
    if (path.nodeID != -1) {
        const Node& node = nodeList.at(path.nodeID);
        return Point(node.longitude, node.latitude);
    }
    // NodeID is -1. So we are working with a building in this situtation.
    ASSERT( path.buildingID != -1 );
    const Building& building = buildingMap.at(path.buildingID);
    return Point(building.wayLon, building.wayLat);
}

PathSegment
PathFinder::getPathOnSameWay(const PathSegment& src,
                             const PathSegment& dest) const {
    // This method is designed to work with 2 segements on the same
    // way.  So ensure that pre-condition is met.
    ASSERT (src.wayID == dest.wayID);    
    // Get the way and points (longitude, latitude) used below.
    const Way& way = wayMap.at(src.wayID);
    // Get the points associated with the starting and ending.
    const Point srcPt = getLatLon(src), destPt = getLatLon(dest);
    
    PathSegment ret = dest;  // segement to return.
    ret.distance    = getDistance(srcPt.second,  srcPt.first,
                                  destPt.second, destPt.first);
    // Handle the simpler bi-directional way case first. We are
    // already done
    if (!way.isOneWay) {
        return ret;
    }

    // Here we handle the more complex case of one-directional ways.
    // But, first get the way we are working with
    ASSERT( way.isOneWay );
    // Find the nearest point on the way for the source and
    // destination to decide if a path between the two is even
    // possible (due to the directionality fo the way).
    const int nearestNode1 = (src.nodeID != -1) ? src.nodeID :
        findNearestNode(way, srcPt.second, srcPt.first);
    const int nearestNode2 = (dest.nodeID != -1) ? dest.nodeID :
        findNearestNode(way, destPt.second, destPt.first);
    // The possible path depends on the nodes in the way.  There are
    // three possible cases that are handled below:
    if (nearestNode1 > nearestNode2) {
        // Nodes are in opposite directions on a 1-way road.  So for
        // this path set large distance to show no path possible
        ret.distance = 1000;
    } else if (nearestNode1 == nearestNode2) {
        // The nearest nodes on the way are the same. So we need a
        // finer distance metric to be able to decide.
        const Node& near   = nodeList.at(nearestNode1);
        const int dist2src = getDistance(near.latitude, near.longitude,
                                           srcPt.second, srcPt.first);
        const int dist2dest  = getDistance(near.latitude, near.longitude,
                                           srcPt.second, srcPt.first);
        if (dist2src < dist2dest) {
            // The nodes are in opposite directions on a 1-way
            // street. So no direct path possib.e
            ret.distance = 1000;
        }
    }
    // Return the path segement with distance setup
    return ret;
}

int
PathFinder::processArgs(int argc, char *argv[]) {
    // Make the arg_record to process command-line arguments.
    ArgParser::ArgRecord arg_list[] = {
        {"--model", "The input model file to be processed",
         &cmdLineArgs.modelFilePath, ArgParser::STRING},
        {"--start-bld", "The ID of the starting building",
         &cmdLineArgs.startBldID, ArgParser::LONG},
        {"--end-bld", "The ID of the destination building",
         &cmdLineArgs.endBldID, ArgParser::LONG},
        {"", "", NULL, ArgParser::INVALID}
    };
    // Process the command-line arguments.
    ArgParser ap(arg_list);
    ap.parseArguments(argc, argv, true);
    // Ensure at least the shape file is specified.
    if (cmdLineArgs.modelFilePath.empty()) {
        std::cerr << "Specify a model file to be processed.\n"
                  << ap << std::endl;
        return 1;
    }
    // Ensure we have some starting and ending building IDs setup
    if ((cmdLineArgs.startBldID == -1) || (cmdLineArgs.endBldID == -1)) {
        std::cerr << "Specify starting and ending building IDs.\n"
                  << ap << std::endl;
        return 2;
    }
    // Things seem fine so far
    return 0;
}

int
PathFinder::loadModel(const std::string& modelFilePath) {
    // Ensure model file is readable.
    std::ifstream model(modelFilePath);
    if (!model.good()) {
        std::cerr << "Error opening model file " << modelFilePath << std::endl;
        return 1;
    }
    // Read boolean as "true"/"false" and not 1/0
    model >> std::boolalpha;
    // Process line-by-line from the model text file to load model
    // data into memory.
    std::string line;
    while (std::getline(model, line)) {
        if (line.empty() || line.front() == '#') {
            continue;  // empty or comment line. Ignore.
        }
        // Wrap string in a stream to ease extractions in if-else below
        std::istringstream is(line);
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
PathFinder::computeNodesWaysList() {
    for (const auto& entry : wayMap) {
        const long wayID = entry.second.id;
        for (const long nodeID : entry.second.nodeList) {
            nodesWaysList.at(nodeID).push_back(wayID);
        }
    }
}

int
PathFinder::findNearestNode(const Way& way, const double latitude,
                            const double longitude) const {
    // We want to iterate only to last-but-one node due to logic in
    // the for-loop below.
    const int NumPoints = way.nodeList.size() - 1;
    // Iterate over pairs of nodes and return index of node.
    for (int i = 0; (i < NumPoints); i++) {
        // Reference to 2 nodes for comparison.
        const Node& n1 = nodeList.at(way.nodeList[i]);
        const Node& n2 = nodeList.at(way.nodeList[i + 1]);
        // Check if given point lies between these nodes.
        if (inBetween(n1.latitude,  n2.latitude,  latitude)  &&
            inBetween(n1.longitude, n2.longitude, longitude)) {
            return i;
        }
    }
    // No valid pair of nodes were found
    return -1;
}

int main(int argc, char *argv[]) {
    PathFinder pf;
    return pf.run(argc, argv);
}

#endif
