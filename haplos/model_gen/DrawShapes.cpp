#ifndef DRAW_SHAPES_CPP
#define DRAW_SHAPES_CPP

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

#include <numeric>
#include "Utilities.h"
#include "DrawShapes.h"

int
DrawShapes::run(int argc, char *argv[]) {
    // Save the command-line args for future reference
    auto concat = [](const std::string& s1, const std::string& s2)
                  { return s1 + " "  + s2; };
    actualCmdLineArgs = std::accumulate(argv, argv + argc, std::string(""),
                                        concat);
    int error = 0;  // Error from various helper methods.
    
    // First process the command-line args and ensure we have
    // necessary arguments for performing various operations.
    if ((error = processArgs(argc, argv)) != 0) {
        return error;  // Error processing command-line args.
    }

    // Next load the community shape file
    if ((error = loadShapeFile()) != 0) {
        return error;  // Error loading community shape file.
    }

    // Next add any annotations the user has specified.
    if (!cmdLineArgs.annotFile.empty()) {
        addAnnotations(cmdLineArgs.annotFile, cmdLineArgs.annotAreaNumCol,
                       cmdLineArgs.annotColorCol);
    }
    
    // Generate the xfig
    generateFig();

    return 0;  // All went well.
}

std::string
DrawShapes::getColumn(const std::string& line, const int column,
                      const char delim) {
    int currCol = 0, startPos = 0;
    while (currCol < column) {
        startPos = line.find(delim, startPos + 1) + 1;
        currCol++;
    }
    int nextTab = line.find(delim, startPos + 1);
    return line.substr(startPos, nextTab - startPos);
}

void
DrawShapes::addAnnotations(const std::string& annotPath,
                           const int areaColIdx, const int colorColIdx) {
    std::ifstream annotTSV(annotPath);
    if (!annotTSV.good()) {
        std::cerr << "Error opening annotation TSV " << annotPath << std::endl;
        return;
    }
    // Process line-by-line of the TSV and update information for each
    // ring.
    for (std::string line; std::getline(annotTSV, line);) {
        // Ignore comment and empty lines.
        if (line.empty() || line.front() == '#') {
            continue;  // comment or empty line
        }
        // Process the annotation on this line.
        const std::string areaID = getColumn(line, areaColIdx);
        if (areaID.empty()) {
            std::cerr << "Unable to extract areaID from column "
                      << areaColIdx << " on line: " << line << std::endl;
                
        }
        ASSERT(!areaID.empty());
        const std::string infoStr = getColumn(line, colorColIdx);
        ASSERT(!infoStr.empty());
        double info = std::stod(infoStr);

        // Find a matching ring based on the area number
        const int ringIdx = shpFile.findRing(areaID,
                                             {"area_numbe", "geoid10"});
        if (ringIdx == -1) {
            std::cerr << "Unable to find ring for areaID " << areaID
                      << " (raw line: " << line << ")\n";
        } else {
            // Add a new annonation ring to the shape file with
            Ring annotRing = shpFile.getRing(ringIdx);
            annotRing.setKind(Ring::POPULATION_RING);
            annotRing.setPopulation(info);
            shpFile.addRing(annotRing);
        }
    }
}

void
DrawShapes::generateFig() {
    // Write the shape information to an XFig file if specified.
    // The community shapes and population grids are to be drawn
    shpFile.genXFig(cmdLineArgs.xfigFilePath,
                    cmdLineArgs.figScale, true);
}

int
DrawShapes::loadShapeFile() {
    // Load data from the shapefile and dbf file.
    if (!shpFile.loadShapes(cmdLineArgs.shapeFilePath,
                            cmdLineArgs.dbfFilePath)) {
        std::cerr << "Error loading shape/DBF file: "
                  << cmdLineArgs.shapeFilePath << " or "
                  << cmdLineArgs.dbfFilePath   << std::endl;
        return 2;
    }
    // Set default lable information for each entry in the ring.
    if (!cmdLineArgs.labelColNames.empty()) {
        shpFile.setLabels(cmdLineArgs.labelColNames);
    }
    // Everything went well
    return 0;
}

int
DrawShapes::processArgs(int argc, char *argv[]) {
    // Make the arg_record to process command-line arguments.
    ArgParser::ArgRecord arg_list[] = {
        {"--shape", "The input shapefile to be drawn",
         &cmdLineArgs.shapeFilePath, ArgParser::STRING },
        {"--dbf", "The associated DBF file to be used for metadata",
         &cmdLineArgs.dbfFilePath, ArgParser::STRING },
        {"--xfig", "The output XFig file",
         &cmdLineArgs.xfigFilePath, ArgParser::STRING},
        {"--scale", "The size of the output map",
         &cmdLineArgs.figScale, ArgParser::INTEGER},
        {"--label-cols", "The names of the columns to use for labels",
         &cmdLineArgs.labelColNames, ArgParser::STRING_LIST},
        {"--annot-tsv", "Annotation TSV for coloring rings",
         &cmdLineArgs.annotFile, ArgParser::STRING},
        {"--annot-area-col", "0-based col index of area ID in annot-tsv",
         &cmdLineArgs.annotAreaNumCol, ArgParser::INTEGER},
        {"--annot-color-col", "0-based col index of data for coloring",
         &cmdLineArgs.annotColorCol, ArgParser::INTEGER},        
        {"", "", NULL, ArgParser::INVALID}
    };
    // Process the command-line arguments.
    ArgParser ap(arg_list);
    ap.parseArguments(argc, argv, true);
    // Ensure at least the shape file is specified.
    if (cmdLineArgs.shapeFilePath.empty() || cmdLineArgs.xfigFilePath.empty()) {
        std::cerr << "Specify a shape file to be drawn and output XFig file.\n";
        return 1;
    }

    if (!cmdLineArgs.annotFile.empty() && cmdLineArgs.annotColorCol == -1) {
        std::cerr << "Specify --anot-color-col to be used.\n";
        return 1;        
    }
    
    // Things seem fine so far
    return 0;
}

int main(int argc, char *argv[]) {
    DrawShapes drawer;
    return drawer.run(argc, argv);
}

#endif
