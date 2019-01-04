 #ifndef POPULATION_RING_GENERATOR_H
#define POPULATION_RING_GENERATOR_H

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

#ifdef HAVE_GDAL
#include <gdal_priv.h>
#endif

#include "ShapeFile.h"

/** Convenience class to process a raster layer with population data
    and generate rings.

    This file is used to process Landscan population data in which the
    data is organized as a Matrix.
 */
class PopulationRingGenerator {
public:
    /** The default and only constructor for this class.

        The constructor initializes the GDAL library and initializes
        all the instance variables to default initial values.
    */
    PopulationRingGenerator();

    /** The destructor.

        Closes the GDAL library and any open files.
     */
    ~PopulationRingGenerator();

    /** Main method to process population GIS data and add shapes to a
        given shapefile.

        \param[in,out] shapes The shapes that contain boundaries for
        which population is to be extracted and created.

        \param[in] popGisFile The GIS data file from where population
        for the shapes are to be read.
     */
    std::vector<Ring> createRings(ShapeFile& shapes,
                                  const std::string& popGisFile,
                                  int& minPop, int& maxPop);
    
protected:
    /** Internal helper method to open given GIS file and setup values
        for various instance variables in this class.
    */
    void openFile(const std::string& path);

    /** Get the grid bounds to process based on the bounds of the
        given shape file.
     */
    void getBounds(const ShapeFile& shapes, int& startX,
                   int& startY, int& endX, int& endY);

    /** Convenience method to see if any grid coordinate lies within a
        any ring in the shapefile.

        \note This method checks 4 different coordinates based on the
        xPixelRes and yPixelRes.
    */
    int getRingForGrid(const ShapeFile& shapes, const double xCoord,
                       const double yCoord) const;

private:
    /** The total number of x-entries in the raster layer */
    int gridWidth;

    /** The total number of y-entries in the raster layer */    
    int gridHeight;

    /** The top-left longitude value, eg: -180 */
    double topLeftX;

    /** The top-left latitude value, eg: -90 */    
    double topLeftY;
    
    /* The the number of degrees represented by each x-cell. For
       example 0.0083333 degrees corresponds to 0.575 miles */
    double xPixelRes;

    /* The the number of degrees represented by each y-cell. For
       example 0.0083333 degrees corresponds to 0.575 miles */    
    double yPixelRes;

#ifdef HAVE_GDAL
    /** A pointer to the GDAL file currently being processed by this
        class.
    */
    GDALDataset* geoDataset;

    /** The raster layer (or band) from where the population data is
        to be read.
    */
    GDALRasterBand* geoBand;
#endif
};

#endif
