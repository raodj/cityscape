#ifndef SHAPE_FILE_CPP
#define SHAPE_FILE_CPP

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

#include <iostream>
#include <string>
#include <climits>
#include <cmath>
#include "ShapeFile.h"
#include "Utilities.h"

ShapeFile::ShapeFile() {
}

ShapeFile::~ShapeFile() {
    // Clear out the rings.
    rings.clear();
}

const Ring&
ShapeFile::getRing(const int index) const {
    return rings.at(index);
}

void
ShapeFile::setLabels(const std::vector<std::string>& colNames) {
    for (Ring& ring : rings) {
        ring.setLabel(ring.getInfo(colNames));
    }
}

bool
ShapeFile::loadShapes(const std::string& shapeFileName,
                      const std::string& dbfFileName,
		      const double scale) {
    DBFHandle dbfFile = NULL;
    std::vector<Ring::Info> metadata;
    if (!dbfFileName.empty()) {
        if ((dbfFile = DBFOpen(dbfFileName.c_str(), "rb")) == NULL) {
            std::cerr << "Unable to open database file " << dbfFileName
                      << "for reading.\n";
            return false;
        }
        metadata = getFieldInfo(dbfFile);
    }
    // Now try and open/process the shape file information.
    SHPHandle shapeFile = SHPOpen(shapeFileName.c_str(), "rb");
    if (shapeFile == NULL) {
        std::cerr << "Unable to open " << shapeFileName << std::endl;
        return false;
    }

    // Load the rings present in the shape file now.
    int numberOfShapes = -1;
    Ring::Kind firstShapeType = Ring::UNKNOWN_RING;
    SHPGetInfo(shapeFile, &numberOfShapes, NULL, NULL, NULL);
    // Iterate over all the shapes in the file and create rings for
    // each shape using the Ring class.
    for (int shapeID = 0; (shapeID < numberOfShapes); shapeID++) {
        // Load the shape from the shape file.
        SHPObject* shape = SHPReadObject(shapeFile, shapeID);
        if (shape->nSHPType == SHPT_NULL) {
            // Ignore NULL shapes
            SHPDestroyObject(shape);
            continue;
        }
        // Figure out the type of this shape
        ASSERT ((shape->nSHPType == SHPT_ARC) ||
                (shape->nSHPType == SHPT_POLYGON));
        const Ring::Kind currShapeType =
            (shape->nSHPType == SHPT_ARC) ? Ring::ARC_RING : Ring::CLOSED_RING;
        // Check the type of shape loaded to ensure it is consistent
        if (firstShapeType == Ring::UNKNOWN_RING) {
            firstShapeType = currShapeType;
        } else if (firstShapeType != currShapeType) {
            std::cerr << "All the shapes in this shape file do not have the "
                      << "same shape type.\nAborting shape file loading.\n";
            return false;
        }
        // Obtain metadata information for all the rings in this shape.
        std::vector<Ring::Info> infoList;
        if (dbfFile != NULL) {
            infoList = getRingInfo(dbfFile, shapeID, metadata);
        }
        size_t oldRingCount = rings.size();
        // Now iterate over each ring in this shape
        for(int partID = 0; (partID < shape->nParts); partID++) {
            // Obtain the number of vertices, starting vertex, and end
            // vertex values.
            const int nextPartIdx = ((partID + 1) < shape->nParts) ? 
                shape->panPartStart[partID + 1] : shape->nVertices;
            const int vertexStart = shape->panPartStart[partID]; 
            const int vertexCount = nextPartIdx - vertexStart;
            // Create the Ring object for this part.
            Ring ring(rings.size(), shapeID, currShapeType, vertexCount,
                      shape->padfX + vertexStart, // x-coords
                      shape->padfY + vertexStart, // y-coords
                      infoList, scale);
            // Add the ring to our list of rings
            rings.push_back(ring);
        }
        // Check for subtraction rings in the newly added ring.
        for (size_t i = oldRingCount; (i < rings.size()); i++) {
            // Check and set subtraction flag for this ring.
            checkSetSubtraction(rings[i]);
        }
        // Now unload the shape as we no longer need it.
        SHPDestroyObject(shape);
    }
    // Everything went on well.
    return true;
}

void
ShapeFile::checkSetSubtraction(Ring& refRing) const {
    for (const Ring& ring : rings) 
    for (size_t ringIdx = 0; (ringIdx < rings.size()); ringIdx++) {
        if ((ring.getShapeID() != refRing.getShapeID()) ||
            (ring.getRingID() == refRing.getRingID())) {
            // Only consider rings from the same shape for overlaps,
            // excluding ourselves
            continue;
        }
        if (ring.contains(refRing)) {
            // Another ring in the list contains this ring.
            refRing.setSubtractionFlag(true);
            return;
        }
    }
}

bool
ShapeFile:: hasExclusion(const int shapeID) const {
    for (const Ring& ring : rings) {
        if ((ring.getShapeID() == shapeID) && ring.isSubtraction()) {
            // Yes, there is at least one ring in this shape that has
            // exclusion/subtraction flag set.
            return true;
        }
    }
    // No rings with exclusion found for the given shape.
    return false;
}

std::vector<Ring::Info>
ShapeFile::getFieldInfo(DBFHandle dbf) const {
    std::vector<Ring::Info> ringInfo;   // return value populated below.
    const int ColCount = DBFGetFieldCount(dbf);
    for(int colID = 0; (colID < ColCount); colID++) {    
        char colTitle[16];
        int width, decimals;
        DBFFieldType colType = DBFGetFieldInfo(dbf, colID, colTitle,
                                               &width, &decimals);
        Ring::Info info = {colType, colTitle, ""};
        ringInfo.push_back(info);
    }
    return ringInfo;
}

std::vector<Ring::Info>
ShapeFile::getRingInfo(DBFHandle dbfFile, int shapeID,
                       std::vector<Ring::Info> fieldInfo) const {
    int colID = 0;
    for (Ring::Info& info : fieldInfo) {
        info.value = DBFReadStringAttribute(dbfFile, shapeID, colID);
        colID++;  // Next logical column number
    }
    return fieldInfo;
}

double
ShapeFile::logScalePop(const double pop, const double minPop,
                       const double maxPop) const {
    double popRatio    = (pop - minPop) / (maxPop - minPop);
    const double range = log10(maxPop / minPop);
    if (range > 1) {
        popRatio = (range + log10(popRatio)) * 100.0 / range;
        popRatio = std::max(0.0, std::min(100.0, popRatio));
        
    } else {
        popRatio *= 100.0;
    }
    return popRatio;
}

void
ShapeFile::genXFig(const std::string& outFileName, const int mapSize,
                   const bool drawCentroid,
                   const std::vector<std::string>& colNames) const {
    XFigHelper fig;
    if (!fig.setOutput(outFileName, true)) {
        std::cerr << "Error writing xfig file " << outFileName << std::endl;
        return;
    }
    // Get the clip bounds to align the top-left corner at zero
    int xClip = 0, yClip = 0;
    getClipBounds(mapSize, xClip, yClip);
    std::cout << "xClip = " << xClip << ", yClip = " << yClip << std::endl;
    // Get the min and max population in the rings to enable color coding.
    double minPop = INT_MAX, maxPop = 0;
    for (const Ring& ring : rings) {
        if ((ring.getKind() == Ring::POPULATION_RING) ||
            (ring.getKind() == Ring::BUILDING_RING)) {
            minPop = std::min(minPop, ring.population);
            maxPop = std::max(maxPop, ring.population);
        }
    }
    // Dump the XFig for each ring in the shapefile
    for (const Ring& ring : rings) {
        int fillColor = -1;
        if (ring.getKind() == Ring::POPULATION_RING) {
            // Convert popRatio to color code from 32
            fillColor = 32 + logScalePop(ring.population, minPop, maxPop);
        }
        ring.printXFig(fig, mapSize, xClip, yClip, drawCentroid, fillColor);
    }
    // Generate a scale for the population if the minPop and maxPop are valid
    if (minPop < maxPop) {
        std::vector<double> tics = {0, minPop};
        for (int frac = 2048; (frac > 2); frac /= 4) {
            const double pop = (maxPop - minPop) / frac;
            const double perc = std::trunc(logScalePop(pop, minPop, maxPop));
            tics.push_back(perc);
            tics.push_back(pop);
        }
        tics.push_back(100);
        tics.push_back(maxPop);
        fig.drawScaleBar(XFIG_SCALE, XFIG_SCALE, 500 * XFIG_SCALE,
                         30 * XFIG_SCALE, tics);
    }
}

void
ShapeFile::getBounds(double& minX, double& minY, double& maxX,
                     double& maxY) const {
    // Initialize the bounds with values of first point.
    const Point p = rings.front().getVertex(0);
    minX = maxX = p.first;
    minY = maxY = p.second;
    for (const Ring& ring : rings) {
	for (int vertex = 0;  (vertex < ring.getVertexCount()); vertex++) {
	    const Point p = ring.getVertex(vertex);
	    minX = std::min(minX, p.first);
	    maxY = std::max(maxY, p.second);
	    maxX = std::max(maxX, p.first);
	    minY = std::min(minY, p.second);
	}
    }
    // Convert latitude/longitude values to xfig units
    std::cout << "maxY = " << maxY << ", minX = " << minX << std::endl;
    std::cout << "minY = " << minY << ", maxX = " << maxX << std::endl;    
}

void
ShapeFile::getClipBounds(const int mapSize, int& xClip, int& yClip) const {
    double minX = 181, maxY = -91;
    double maxX = -181, minY = 91;
    for (const Ring& ring : rings) {
	for (int vertex = 0;  (vertex < ring.getVertexCount()); vertex++) {
	    Point p = ring.getVertex(vertex);
	    minX = std::min(minX, p.first);
	    maxY = std::max(maxY, p.second);
	    maxX = std::max(maxX, p.first);
	    minY = std::min(minY, p.second);
	}
    }
    // Convert latitude/longitude values to xfig units
    std::cout << "maxY = " << maxY << ", minX = " << minX << std::endl;
    std::cout << "minY = " << minY << ", maxX = " << maxX << std::endl;
    getXYValues(maxY, minX, xClip, yClip, mapSize * XFIG_SCALE, false);
}

void
ShapeFile::addRing(const Ring& ring) {
    rings.push_back(ring);
}

void
ShapeFile::addRings(const std::vector<Ring>& addlRings) {
    for (const Ring& ring : addlRings) {
        rings.push_back(ring);
    }
}

#endif
