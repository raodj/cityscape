#ifndef SHP_BLD_CATALOG_H
#define SHP_BLD_CATALOG_H

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

#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include "ShapeFile.h"
#include "OSMData.h"

/**
 * Synonym to streamline working with the unordered map
 */
using CatalogMap = std::unordered_map<std::string, std::vector<long>>;

/**
 * Manages shape -> building mappings making it easier to find
 * buildings in a given shape.
 */
class ShpBldCatalog {
public:
    /**
     * Builds the internal catalog for rapid building look up in
     * rings.  This method loads the shapes and builds the assignment
     * of buildings to shapes.
     *
     * \param[in] shpFile Path to the shape file from where the shapes
     * are to be loaded.
     *
     * \param[in] dbfFile Path to the file that contains metadata for
     * the shapes.
     *
     * \param[in] model The model who's buildings are to be looked-up.
     */
    void buildCatalog(const std::string& shpFile, const std::string& dbfFile,
                      const OSMData& model);

    /**
     * Obtain an random building in a given area.
     *
     * \param[in] areaID The ID of the area from where a random
     * building ID is to be returned.
     *
     * \return The building ID if areaID is valid. Otherwise -1.
     *
     * \note This is a O(1) method but it does take time to compute a
     * random number.
     */
    long getRndBldID(const std::string& areaID) const;
    
private:
    /** The shape file containing rings loaded from census tracts
     * boundaries specified associated with the model.
     */
    ShapeFile shpFile;

    /**
     * This unordered map maintains the list of buildings for a given
     * ring identifier. The key is the ring-identifier. The vector
     * contains the list of buildings.  This is created 
     */
    CatalogMap catalog;

    /**
     * Random number generator used to randomly select a building ID
     * in a given area.
     */
    // mutable std::default_random_engine rndGen;
};

#endif
