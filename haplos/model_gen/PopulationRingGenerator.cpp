 #ifndef POPULATION_RING_GENERATOR_CPP
#define POPULATION_RING_GENERATOR_CPP

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

#include <stdexcept>
#include <iostream>
#include "PopulationRingGenerator.h"

PopulationRingGenerator::PopulationRingGenerator() {
    // Register readers for differen file-formats in GDAL Library
    GDALAllRegister();
    // Initialize instance variables
    geoDataset = NULL;
}

PopulationRingGenerator::~PopulationRingGenerator() {
    if (geoDataset != NULL) {
        delete geoDataset;
        geoDataset = NULL;
    }
    // Allow GDAL to perform final cleanup
    GDALDestroyDriverManager();
}

std::vector<Ring>
PopulationRingGenerator::createRings(ShapeFile& shapes,
                                     const std::string& popGisFile,
                                     int& minPop, int& maxPop) {
    // Initialize min and max population
    minPop = INT_MAX;
    maxPop = 0;
    // The new set of rings being created
    std::vector<Ring> popRings;
    // First setup the GIS data file to load information.
    openFile(popGisFile);
    // Next find out the bounds of the shapes in the shape file so
    // that we can just iterate over that range in the GIS data file
    // and save a lot of compute time.
    int startX = 0, startY = 0, endX = 0, endY = 0;
    getBounds(shapes, startX, startY, endX, endY);
    // Setup the starting latitude and longitude values.
    double currLat = topLeftY + (startY * yPixelRes);
    // Iterate over the bounds and create rings if at least one point
    // of the grid is inside a ring in the shapes.
    int ringID = shapes.getRings().size();
    std::vector<float> grid(gridWidth);
    for (int currY = startY; (currY <= endY); currY++, currLat += yPixelRes) {
        // Read the line of GIS data for the current row
        geoBand->RasterIO(GF_Read, 0, currY, gridWidth, 1, &grid[0],
                          gridWidth, 1, GDT_Float32, 0, 0);
        // Reset the current longitude for each iteration.
        double currLon = topLeftX + (startX * xPixelRes);
        for (int currX = startX; (currX <= endX); currX++,
                 currLon += xPixelRes) {
            // Create ring for this location if an existing ring
            // contains this entry.
            const int pop = grid[currX];
            if (pop < 1) {
                continue;  // Empty population grid.
            }
            const int area = getRingForGrid(shapes, currLon, currLat);
            if (area != -1) {
                const Ring& ring = shapes.getRing(area);
                std::vector<Ring::Info> infoList = {
                    {0, "currLat",    std::to_string(currLat)},
                    {0, "currLon",    std::to_string(currLon)},
                    {0, "population", std::to_string(pop)},
                    {0, "area_numbe", ring.getInfo("area_numbe")},
                    {1, "community",  ring.getInfo("community")},
                    {0, "popRingID",  std::to_string(ringID)}
                };
                popRings.push_back(Ring(Point(currLon, currLat),
                                        Point(currLon + xPixelRes,
                                              currLat + yPixelRes), pop,
                                        ringID++, -1, infoList));
                // Track minimum and maximum population values.
                minPop = std::min(minPop, pop);
                maxPop = std::max(maxPop, pop);
            }
        }
    }
    // Return the created population rings.
    return popRings;
}

int
PopulationRingGenerator::getRingForGrid(const ShapeFile& shapes,
                                        const double xCoord,
                                        const double yCoord) const {
    // Get a immutable reference for convenience.
    const std::vector<Ring>& rings = shapes.getRings();
    for (size_t i = 0; (i < rings.size()); i++) {
        const Ring& ring = rings[i];
        if (ring.contains(xCoord, yCoord) ||
            ring.contains(xCoord + xPixelRes, yCoord) ||
            ring.contains(xCoord + xPixelRes, yCoord + yPixelRes) ||
            ring.contains(xCoord, yCoord + yPixelRes) ||
            ring.contains(xCoord + xPixelRes / 2, yCoord + yPixelRes / 2)) {
            return i;  // Found a containing ring.
        }
            
    }
    return -1;  // No containing ring found.
}

void
PopulationRingGenerator::getBounds(const ShapeFile& shapes, int& startX,
                                   int& startY, int& endX, int& endY) {
    // First get the bounds of all the shapes from the shape file
    double minX, minY, maxX, maxY;
    shapes.getBounds(minX, minY, maxX, maxY);
    // Convert convert the latitude and longitude to corresponding x &
    // y coordinates in the rater matarix based on the resolution.
    startX = ((minX - topLeftX) / xPixelRes) - 1;
    endX   = ((maxX - topLeftX) / xPixelRes) + 1;
    startY = ((maxY - topLeftY) / yPixelRes) - 1;
    endY   = ((minY - topLeftY) / yPixelRes) + 1;
    // Print information to help with troubleshooting
    std::cout << "[INFO] Processing grid from: (" << startX << ", "
              << startY << ") to (" << endX << ", " << endY << ")"
              << std::endl;
}


void
PopulationRingGenerator::openFile(const std::string& popGisFile) {
    // Attempt to open the gis file
    GDALDataset* geoDataset  = (GDALDataset *) GDALOpen(popGisFile.c_str(),
                                                        GA_ReadOnly);
    // Hopefully we succeeded
    if (geoDataset == NULL) {
        throw std::runtime_error("GDAL was unable to open the GIS file");
    }
    // Print some information about the GDAL driver being used.
    std::cout << "[INFO] Using Driver: "
              << geoDataset->GetDriver()->GetDescription() << " -- "
              << geoDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME)
              << std::endl;
    // Make sure that the GIS file only has one band (layer?), as we
    // cannot process more than that.
    if (geoDataset->GetRasterCount() != 1) {
        throw std::invalid_argument("The specified GIS file contains more "
                                    "than one band, which can't be processed");
    }
    // Now, get the actual Raster Band so we can get the data we really want
    geoBand = geoDataset->GetRasterBand(1);
    std::cout << "[INFO] Band Data Type: " <<
        GDALGetDataTypeName(geoBand->GetRasterDataType()) << std::endl;
    // Set up the "no data value" to zero
    geoBand->SetNoDataValue(0);

    // Finally load metadata about the GIS file into instance variables.
    // Figure out the width and height, in columns/rows
    gridWidth  = geoDataset->GetRasterXSize();
    gridHeight = geoDataset->GetRasterYSize();
    std::cout << "[INFO] Size: (" << gridWidth << ", "  << gridHeight << ")"
              << std::endl;
    // Get the so-called "Geographical Transform" data which indicates
    // the starting origin of the gridded data, as well as its
    // resolution. Populating geoTransform also allows for the use of
    // the methods getTopLeftX() and such, which makes more sense than
    // accessing the array using magic numbers.
    double geoTransform[6];
    if (geoDataset->GetGeoTransform(geoTransform) != CE_None) {
        throw std::runtime_error("Could not read geographic transformation "
                                 "data, unable to process file");
    }
    // Setup values for various instance variables.
    topLeftX  = geoTransform[0];
    topLeftY  = geoTransform[3];
    xPixelRes = geoTransform[1];
    yPixelRes = geoTransform[5];
    std::cout << "[INFO] Top-Left: (" << topLeftX << ", "
              << topLeftY << ")" << std::endl;
    std::cout << "[INFO] Pixel Resolution: (" << xPixelRes << ", "
              << yPixelRes << ")" << std::endl;    
}

#endif
