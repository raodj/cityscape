#ifndef PATH_FINDER_TESTER_CPP
#define PATH_FINDER_TESTER_CPP

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

#include <set>
#include <chrono>
#include <numeric>
#include "Utilities.h"
#include "PathFinder.h"
#include "PathFinderTester.h"

int
PathFinderTester::processArgs(int argc, char *argv[]) {
    // Save the command-line arguments for future reference.
    auto concat = [](std::string s1, std::string s2) { return s1 + " " + s2; };
    cmdLineArgs.fullCmdLine = std::accumulate(argv, argv + argc,
                                              cmdLineArgs.fullCmdLine, concat);
    // Make the arg_record to process command-line arguments.
    ArgParser::ArgRecord arg_list[] = {
        {"--model", "The input model file to be processed",
         &cmdLineArgs.modelFilePath, ArgParser::STRING},
        {"--start-bld", "The ID of the starting building",
         &cmdLineArgs.startBldID, ArgParser::LONG},
        {"--end-bld", "The ID of the destination building",
         &cmdLineArgs.endBldID, ArgParser::LONG},
        {"--xfig", "Optional output XFig file",
         &cmdLineArgs.xfigFilePath, ArgParser::STRING},  
        {"--append", "Optional output XFig file (after appending)",
         &cmdLineArgs.baseFigPath, ArgParser::STRING},   
        {"--draw", "Specify drawing mode: all or nearby (default: all)",
         &cmdLineArgs.drawMode, ArgParser::STRING},      
        {"--scale", "The size of the output map",
         &cmdLineArgs.figScale, ArgParser::INTEGER},
        {"--search-dist", "Minimum search distance (in miles) to find nodes",
         &cmdLineArgs.minDist, ArgParser::DOUBLE},
        {"--search-scale", "Extra distance/mile to search for path",
         &cmdLineArgs.distScale, ArgParser::DOUBLE},
        {"--best-time", "Return path based on fastest time",
         &cmdLineArgs.useTime, ArgParser::BOOLEAN},
        {"--rnd-test", "Run a given number of random tests",
         &cmdLineArgs.rndTestCount, ArgParser::INTEGER},
        {"--rnd-start", "Starting index for setting seed",
         &cmdLineArgs.rndSeed, ArgParser::INTEGER},
        {"--check-entry", "Check entry is correct for all buildings",
         &cmdLineArgs.checkEntries, ArgParser::BOOLEAN},
        {"--check-noway", "Print ways with no connections to others",
         &cmdLineArgs.printNoWays, ArgParser::BOOLEAN},
        {"--stdio", "Use standard I/O streams to read building IDs",
         &cmdLineArgs.useStdIO, ArgParser::BOOLEAN},
        {"--shape", "The input shapefile to be drawn",
         &cmdLineArgs.shapeFilePath, ArgParser::STRING },
        {"--dbf", "The associated DBF file to be used for metadata",
         &cmdLineArgs.dbfFilePath, ArgParser::STRING },
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
    if ((cmdLineArgs.rndTestCount < 0) && (!cmdLineArgs.useStdIO) &&
        ((cmdLineArgs.startBldID == -1) || (cmdLineArgs.endBldID == -1))) {
        std::cerr << "Specify starting and ending building IDs.\n"
                  << ap << std::endl;
        return 2;
    }
    // Things seem fine so far
    return 0;
}

double
PathFinderTester::getDistance(const long startBldID, const long endBldID) const {
    const Building& stBld  = osmData.buildingMap.at(startBldID);
    const Building& endBld = osmData.buildingMap.at(endBldID);
    return ::getDistance(stBld.wayLat, stBld.wayLon,
                         endBld.wayLat, endBld.wayLon);
}


void
PathFinderTester::runRndTest(const long startBldID, const long endBldID) const {
    std::cout << startBldID << " -- " << endBldID << " : ";
    // Namespaces associated with standard time measurement classes below.
    using namespace std::chrono;
    // Record start time
    const high_resolution_clock::time_point start =
        high_resolution_clock::now();
    PathFinder pf(osmData);
    Path path = pf.findBestPath(startBldID, endBldID,
                                cmdLineArgs.useTime,    cmdLineArgs.minDist,
                                cmdLineArgs.distScale);
    if ((path.size() <= 2) && (cmdLineArgs.minDist != -1)) {
        // If path could not be found, then re-run without limiting search
        std::cout << " retry : ";
        PathFinder pf(osmData);        
        path = pf.findBestPath(startBldID, endBldID, cmdLineArgs.useTime);
    }
    const high_resolution_clock::time_point end = high_resolution_clock::now();
    // Compute elapsed time.
    const duration<double> computeTime =
        duration_cast<duration<double>>(end - start);
    std::cout << (path.empty() ? "Failed" : "Success")
              << " ("     << path.size() << " nodes, time: "
              << computeTime.count()
              << " secs)";
    if (!path.empty()) {
        if (cmdLineArgs.useTime) {
            // The primary unit is time and altMetric is distance
            std::cout << ", dist: " << (path.back().distance * 60)
                      << " mins, altMetric: " << path.back().altMetric
                      << " miles";
        } else {
            // The primary unit is distance and altMetric is time
            std::cout << ", dist: " << path.back().distance
                      << " miles, altMetric: "
                      << (path.back().altMetric * 60)
                      << " mins";            
        }
    }
    std::cout << ", explored: " << pf.getExploredNodeCount() << " nodes, "
              << "straight line distance: "
              << getDistance(startBldID, endBldID) << " miles\n";
}

std::vector<long>
PathFinderTester::getBuildingIDs() const {
    // The total number of buildings to choose from.
    const int BldCount = osmData.buildingMap.size();
    // Create a vector of building IDs to ease random selection
    std::vector<long> bldIDs;
    bldIDs.reserve(BldCount);  // reserve memory.
    // Add the building IDs to the list of building
    for (auto entry : osmData.buildingMap) {
        bldIDs.push_back(entry.first);  // Add IDs
    }
    return bldIDs;
}

void
PathFinderTester::runRndTests(int testCount) const {
    // Get the building IDs that we are working with.
    const std::vector<long> bldIDs = getBuildingIDs();
    const int BldCount             = bldIDs.size();
    // Print the command-line being used for testing for reference
    std::cout << "Running: " << cmdLineArgs.fullCmdLine << std::endl;
    // Run the given number of tests
    for (int i = 0; (i < testCount); i++) {
        // Randomly select the indexs of a pair of buildings
        srand(i + cmdLineArgs.rndSeed);
        const int srtBldIdx = rand() % BldCount;
        const int endBldIdx = rand() % BldCount;
        // Run the test on this pair
        runRndTest(bldIDs[srtBldIdx], bldIDs[endBldIdx]);
    }
}

void
PathFinderTester::checkEntries() const {
    // Get the building IDs that we are working with.
    const std::vector<long> bldIDs = getBuildingIDs();
    // Create a path finder object for testing
    const PathFinder pf(osmData);
    // Run the given number of tests
    for (size_t i = cmdLineArgs.rndSeed; (i < bldIDs.size()); i++) {
        // Get the building to be checked (for convenience)
        const Building& bld = osmData.buildingMap.at(bldIDs[i]);
        const Way&      way = osmData.wayMap.at(bld.wayID);
        const int nodeIdx   = pf.findNearestNode(way, bld.wayLat, bld.wayLon);
        if ((nodeIdx < 0) || (nodeIdx > (int) way.nodeList.size())) {
            std::cout << "Invalid entry way for: ";
            bld.write(std::cout);
        }
    }
}

void
PathFinderTester::printDisconnectedWays() {
    // Process each way in the model. For each way check to see if
    // there is at least one node that was 2 way-IDs in it. If not, it
    // is a disconnected way.
    for (const auto& wayEntry : osmData.wayMap) {
        // Get the way we are working with.
        const Way& way = wayEntry.second;
        // For each node in the way check to see if it has two other
        // ways associated with it.
        bool connected = false;
        for (const long nodeID : way.nodeList) {
            // Get the list of ways associated with this node.
            const std::vector<long> nodeWays = osmData.nodesWaysList.at(nodeID);
            // Quick check for most common case where node is only on
            // 1 way.
            if (nodeWays.size() < 2) {
                continue;  // no intersections here.
            }
            // Quicker check to see if this node could have 2 ways
            // intersecting.
            if ((nodeWays.size() == 2) && (nodeWays[0] != nodeWays[1])) {
                connected = true;  // Found intersection/connections.
                break;             // Short-circuit further checks.
            }
            // Create a set of unique ways to detect number of unique
            // ways associated with this node.  
            std::set<long> uniqWayIDs(nodeWays.begin(), nodeWays.end());
            if (uniqWayIDs.size() > 1) {
                connected = true;  // Found intersection/connections.
                break;             // Short-circuit further checks.
            }
        }
        // Check if this way is connected.
        if (!connected) {
            std::cout << "Way: " << way.id << " (kind: "
                      << way.kind << ", nodes: " << way.nodeList.size()
                      << ") is disconnected.\n";
        }
    }
}

void
PathFinderTester::processStdIO() {
    // The pair of buildings for which route is to be computed.
    long startBldID, endBldID;
    // Keep processing pairs of buildings.
    while (std::cin >> startBldID >> endBldID) {
        // Compute the path between the pairs of buildings
        PathFinder pf(osmData);
        Path path = pf.findBestPath(startBldID, endBldID,
                                    cmdLineArgs.useTime, cmdLineArgs.minDist,
                                    cmdLineArgs.distScale);
        // Print the detailed path
        pf.printDetailedPath(path);
    }
}

int
PathFinderTester::run(int argc, char *argv[]) {
    int error = 0;  // Error from various helper methods.
    // First process the command-line args and ensure we have
    // necessary arguments for performing various operations.
    if ((error = processArgs(argc, argv)) != 0) {
        return error;  // Error processing command-line args.
    }
    // Next load the community shape file
    if ((error = osmData.loadModel(cmdLineArgs.modelFilePath)) != 0) {
        return error;  // Error loading community shape file.
    }
    // Perform entry checks if requested
    if (cmdLineArgs.checkEntries) {
        checkEntries();
    }
    // Print disconnected ways if requested
    if (cmdLineArgs.printNoWays) {
        printDisconnectedWays();
    }
    // Load census tracts and assign buildings to census tracts, if a
    // census shape file is given.
    if (!cmdLineArgs.shapeFilePath.empty()) {
        shpBldCatalog.buildCatalog(cmdLineArgs.shapeFilePath,
                                   cmdLineArgs.dbfFilePath, osmData);
    }
    
    // Run random tests or one given test.
    if (cmdLineArgs.rndTestCount == -1) {
        // If user asks to use standard I/O then do that.
        if (cmdLineArgs.useStdIO) {
            processStdIO();
            return 0;
        }
        // Ensure starting and ending building ID's are valid.
        if ((osmData.buildingMap.find(cmdLineArgs.startBldID) ==
             osmData.buildingMap.end()) ||
            (osmData.buildingMap.find(cmdLineArgs.endBldID) ==
             osmData.buildingMap.end())) {
            std::cerr << "Starting or ending building not found in model.\n";
            return 3;
        }
        // Print the resulting path segement between the two
        PathFinder pf(osmData);
        Path path = pf.findBestPath(cmdLineArgs.startBldID,
                                    cmdLineArgs.endBldID,
                                    cmdLineArgs.useTime, cmdLineArgs.minDist,
                                    cmdLineArgs.distScale);
        // Draw the path as xfig
        std::cout << path;    
        pf.generateFig(path, cmdLineArgs.xfigFilePath, cmdLineArgs.figScale,
                       cmdLineArgs.drawMode);
    } else {
        // Run random tests
        runRndTests(cmdLineArgs.rndTestCount);
    }
    // Everything went well
    return 0;
}

int main(int argc, char *argv[]) {
    PathFinderTester pft;
    return pft.run(argc, argv);
}

#endif
