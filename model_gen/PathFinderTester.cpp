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

#include "PathFinder.h"
#include "PathFinderTester.h"

int
PathFinderTester::processArgs(int argc, char *argv[]) {
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
        {"--scale", "The size of the output map",
         &cmdLineArgs.figScale, ArgParser::INTEGER},
        {"--search-dist", "Minimum search distance (in miles) to find nodes",
         &cmdLineArgs.minDist, ArgParser::DOUBLE},
        {"--search-scale", "Extra distance/mile to search for path",
         &cmdLineArgs.distScale, ArgParser::DOUBLE},        
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
    Path path = pf.findBestPath(cmdLineArgs.startBldID, cmdLineArgs.endBldID,
                                cmdLineArgs.minDist, cmdLineArgs.distScale);
    // Draw the path as xfig
    std::cout << path;    
    pf.generateFig(path, cmdLineArgs.xfigFilePath, cmdLineArgs.figScale);
    // Everything went well
    return 0;
}

int main(int argc, char *argv[]) {
    PathFinderTester pft;
    return pft.run(argc, argv);
}

#endif
