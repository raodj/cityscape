#ifndef SHAPE_FILE_H
#define SHAPE_FILE_H

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

#include <vector>
#include "Ring.h"
#include "shapefil.h"

/** A convenience wrapper around a shape file.

    This class is meant to serve as a convenient wrapper around a
    shape file data structure for processing contents of a shape file.
    The shapes in the shape file are loaded and stored a set of Ring
    objects to ease further processing.
*/
class ShapeFile {
public:
    ShapeFile();
    ~ShapeFile();
    /**
     * \param [in] checkSubtractRing When loading PUMA data, it was
     * observed that checks to detect subtraction rings was taking a
     * long time.  Consequently, this flag was added to permit caller
     * to avoid the O(n^2) check for subtraction-ring checks.
     *
     * \param[in] correctConvex When loading PUMA data, some of the
     * rings have overlapping points which causes some of the
     * boost::geometry algorithms to fail due to conave nature of
     * shape.  If this flag is true, then this method attempts to
     * detect and correct such points.
     */
    bool loadShapes(const std::string& shapeFileName,
		    const std::string& dbfFileName = "",
		    const double scale = 1.0,
                    const bool checkSubtractRing = true,
                    const bool correctConvex = false);
    inline int getRingCount() const { return rings.size(); }
    const Ring& getRing(const int index) const;
    int findRing(const std::string& info,
                 const std::vector<std::string>& colNames) const;
    bool hasExclusion(const int shapeID) const;

    void genXFig(const std::string& outFileName,
                 const int mapSize = 4096, const bool drawCentroid = false,
                 const std::vector<std::string>& colNames = {},
                 const bool drawScaleBar = true) const;
    
    void genXFig(XFigHelper& fig, int& xClip, int& yClip,
                 const int mapSize = 4096, const bool drawCentroid = false,
                 const std::vector<std::string>& colNames = {},
                 const bool drawScaleBar = true,
                 const std::string& outFileName = "",
                 const int startLayer = 50) const;
    
    void getBounds(double& minX, double& minY, double& maxX,
                   double& maxY) const;
    void getClipBounds(const int mapSize, int& xClip, int& yClip) const;
    void setLabels(const std::vector<std::string>& colNames);

    void addRing(const Ring& ring);
    void addRings(const std::vector<Ring>& rings);

    const std::vector<Ring>& getRings() const { return rings; }

    double logScalePop(const double pop, const double minPop,
                       const double maxPop) const;

protected:
    bool loadStatus(const std::string& dbfFileName,
                    std::vector<std::string>& statusList);
    void checkSetSubtraction(Ring& ring) const;

    int getColID(DBFHandle dbfFile, const std::string& colName);

    /** Convenience method to read field informatin from the given DBF file.

        The field information is the same for all the rings.  This
        method loads the field information in to a list and returns
        it.

        \return The field information associated with this ring.
    */
    std::vector<Ring::Info> getFieldInfo(DBFHandle dbFile) const;

    /**
     */
    std::vector<Ring::Info>
    getRingInfo(DBFHandle dbfFile, int shapeID,
                std::vector<Ring::Info> fieldInfo) const;

private:
    std::vector<Ring> rings;

};

#endif
