#ifndef DRAW_BUILDINGS_CPP
#define DRAW_BUILDINGS_CPP

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
#include <climits>
#include "Utilities.h"
#include "DrawBuildings.h"
#include "ShapeFile.h"
#include "OSMData.h"

int
DrawBuildings::run(int argc, char *argv[]) {
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

    // Next open an XFig file for drawing all of the necesssary
    // information.
    XFigHelper xfig;
    if (!xfig.setOutput(cmdLineArgs.xfigFilePath, true)) {
        std::cerr << "Error writing to fig file: "
                  << cmdLineArgs.xfigFilePath << std::endl;
        return 10;
    }
    
    // Next load and draw the shape files
    if ((error = drawShapeFiles(xfig, cmdLineArgs.shapeFilePaths,
                                cmdLineArgs.dbfFilePaths)) != 0) {
        return error;  // Error loading community shape file.
    }

    // Next load the model from the specified model file.
    OSMData model;
    if ((error = model.loadModel(cmdLineArgs.modelFilePath)) != 0) {
        std::cerr << "Error loading model from: " << cmdLineArgs.modelFilePath
                  << std::endl;
        return error;
    }

    // Draw the buildings based on specified information.
    drawBuildings(model, xfig, cmdLineArgs.key_info);
    
    return 0;  // All went well.
}

int
DrawBuildings::getInfo(const Building& bld, const std::string& infoKey) const {
    return bld.getInfo(infoKey);
}

void
DrawBuildings::drawBuildings(const OSMData& model, XFigHelper& fig,
                             const std::string& infoKey) {
    // In order to repurpose the features of drawing scale etc, we
    // just create a shape file object and add buildings to it as
    // rings.
    ShapeFile bldsSF;   // buildings shape file
    for (const auto bldEntry : model.buildingMap) {
        const Building& bld = bldEntry.second;  // Reference to building
        // Create a ring for this building
        bldsSF.addRing(getBldRing(bldEntry.first, bld, infoKey));
    }
}

Ring
DrawBuildings::getBldRing(int bldId, const Building& bld,
                          const std::string& infoKey) const {
    // Get info to use as population for the building
    const int info = bld.getInfo(infoKey);    
    // Create the top-left and bottom-right points.
    const Point topLeft(bld.topLon, bld.topLat),
        botRight(bld.botLon, bld.botLat);
    const std::string entInfo = "bldId: " + std::to_string(bld.id) +
        "--" + infoKey + ": " + std::to_string(info);
    const std::vector<Ring::Info> infoList = {{1, "info",  entInfo }};
    // Create a ring for this building.
    return Ring(topLeft, botRight, info, bldId, bldId, infoList);
}

std::string
DrawBuildings::getColumn(const std::string& line, const int column,
                      const char delim) {
    int currCol = 0, startPos = 0;
    while (currCol < column) {
        startPos = line.find(delim, startPos + 1) + 1;
        currCol++;
    }
    int nextTab = line.find(delim, startPos + 1);
    return line.substr(startPos, nextTab - startPos);
}

int
DrawBuildings::drawShapeFiles(XFigHelper& fig, const StringList& shapeFiles,
                              const StringList& dbfFiles,
                              const int figScale, int startShapeLevel) {
    // The first shapefile becomes the primary. Other shape files are
    // intersected with this file to make the figure better.
    std::vector<ShapeFile> shapes;

    // The folllowing minXclip and minYclip are updated in loop below
    // to track the minimum values from all the shape files being
    // drawn.
    int minXclip = INT_MAX, minYclip = INT_MAX;

    // Below we accummulate all the shapes into a vector so that we
    // can determine the overall XClip and YClip values to know what
    // coordinate get's anchored to the top-left corner in XFig.  This
    // is needed because different shape files can end up having
    // different bounds.
    for (size_t i = 0; (i < shapeFiles.size()); i++) {
        // A temporary object to load shapes.
        ShapeFile shpFile;
        // Load data from the shapefile and dbf file.
        if (!shpFile.loadShapes(shapeFiles[i], dbfFiles[i])) {
            std::cerr << "Error loading shape/DBF file: "
                      << shapeFiles[i] << " or " << dbfFiles[i] << std::endl;
            return 3;
        }
        // Set default label information for each entry in the ring.
        if (!cmdLineArgs.labelColNames.empty()) {
            shpFile.setLabels(cmdLineArgs.labelColNames);
        }
        // If this is not the first shape file then intersect with
        // shapes in the first one to retain only relevant shapes to
        // keep the figure compact.
        if (!shapes.empty()) {
            std::cout << "Intersecting shape file " << shapeFiles[i]
                      << std::endl;
            shpFile = intersections(shapes.front(), shpFile);
        }
        // Add the new shape to the shapes to be drawn.
        shapes.push_back(shpFile);

        // Update the clipping bounds based on the new shape
        int xClip = -1, yClip = -1;
        shpFile.getClipBounds(figScale, xClip, yClip);
        minXclip = std::min(minXclip, xClip);
        minYclip = std::min(minYclip, yClip);
    }

    // Now write all the shapes to the specified xfig with a fixed
    // clipping bounds.
    const std::vector<int> Colors = { BLACK, RED, BLUE, GREEN, CYAN };
    for (size_t i = 0; (i < shapes.size()); i++) {
        shapes[i].genXFig(fig, minXclip, minYclip, cmdLineArgs.figScale,
                          false, {}, false, shapeFiles[i],
                          startShapeLevel + (i * 10), Colors.at(i));
    }
    // Everything went well
    return 0;
}

int
DrawBuildings::processArgs(int argc, char *argv[]) {
    // Make the arg_record to process command-line arguments.
    ArgParser::ArgRecord arg_list[] = {
        {"--shapes", "List of shape files to be drawn",
         &cmdLineArgs.shapeFilePaths, ArgParser::STRING_LIST },
        {"--dbfs", "List of associated DBF file to be used for metadata",
         &cmdLineArgs.dbfFilePaths, ArgParser::STRING_LIST },
        {"--xfig", "The output XFig file",
         &cmdLineArgs.xfigFilePath, ArgParser::STRING},
        {"--scale", "The size of the output map",
         &cmdLineArgs.figScale, ArgParser::INTEGER},
        {"--label-cols", "Shapes metadata columns to use for labels",
         &cmdLineArgs.labelColNames, ArgParser::STRING_LIST},
        {"--key-info", "Key building info to plot [households, people]",
         &cmdLineArgs.key_info, ArgParser::STRING},
        {"--model", "The model text file for building data",
         &cmdLineArgs.modelFilePath, ArgParser::STRING},
        {"", "", NULL, ArgParser::INVALID}
    };
    // Process the command-line arguments.
    ArgParser ap(arg_list);
    ap.parseArguments(argc, argv, true);
    // Ensure at least the model file and XFig files are specified.
    if (cmdLineArgs.modelFilePath.empty() || cmdLineArgs.xfigFilePath.empty()) {
        std::cerr << "Specify a model file and output XFig file.\n";
        return 1;
    }

    // Ensure that the number of shapes and dbf files match up
    if (cmdLineArgs.shapeFilePaths.size() != cmdLineArgs.dbfFilePaths.size()) {
        std::cerr << "The number of shapes & dbf files do not match\n";
        return 2;
    }

    // Things seem fine so far
    return 0;
}

ShapeFile
DrawBuildings::intersections(const ShapeFile& primary,
                             const ShapeFile& secondary) const {
    // The resulting shapefile with shapes from secondary that
    // intersect with the primary.
    ShapeFile intersection;
#pragma omp parallel for schedule(guided)
    for (int secIdx = 0; (secIdx < secondary.getRingCount()); secIdx++) {
        // Get the ring that we are working with
        const Ring& secRing = secondary.getRing(secIdx);
        // Check intersection between secondary ring with all primary rings.
        for (int priIdx = 0; (priIdx < primary.getRingCount()); priIdx++) {
            const Ring& priRing = primary.getRing(priIdx);
            if (priRing.contains(secRing) || secRing.intersects(priRing)) {
                // Found an intersection. Copy the secondary ring to
                // our resulting shape file.
#pragma omp critical (intersect)
                intersection.addRing(secRing);
            }
        }
    }  // omp parallel

    // Return intersecting shapes back.
    return intersection;
}

int main(int argc, char *argv[]) {
    DrawBuildings drawer;
    return drawer.run(argc, argv);
}

#endif
