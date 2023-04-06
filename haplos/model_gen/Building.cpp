#ifndef BUILDING_CPP
#define BUILDING_CPP

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
#include "Building.h"
#include "Utilities.h"

Building
Building::create(const Ring& ring, int id, int levels, int population,
                 int attributes) {
    // The building object being created by this method.
    Building bld;
    // Set the standard attributes
    bld.id         = id;
    bld.levels     = levels;
    bld.population = population;
    bld.attributes = attributes;
    bld.isHome     = false;
    // Compute the bounds and square-footage for this building.
    bld.sqFootage  = ring.getArea() * 27878000.0;
    ASSERT(bld.sqFootage > 0);
    // Get bounds for this building
    Point topLeft, botRight;
    ring.getBounds(topLeft, botRight);
    // Setup the bounds in the building object
    bld.topLon     = topLeft.first;
    bld.topLat     = topLeft.second;
    bld.botLon     = botRight.first;
    bld.botLat     = botRight.second;
    // Clear out information about ways
    bld.wayID      = -1;
    bld.wayLat     = -1;
    bld.wayLon     = -1;
    bld.pumaID     = -1;
    // Return the building
    return bld;
}

Building
Building::create(int id, int sqFootage, long wayID,
                 double wayLat, double wayLon, double topLat,
                 double topLon, double botLat, double botLon,
                 int popRingID, bool isHome, int levels, int population,
                 int pumaID) {
    // The building object being created by this method.
    Building bld;
    // Set the standard attributes
    bld.id         = id;
    bld.levels     = levels;
    bld.population = population;
    bld.attributes = popRingID;
    bld.isHome     = isHome;
    // Compute the bounds and square-footage for this building.
    bld.sqFootage  = sqFootage;
    ASSERT(bld.sqFootage > 0);
    // Setup the bounds in the building object
    bld.topLon     = topLon;
    bld.topLat     = topLat;
    bld.botLon     = botLon;
    bld.botLat     = botLat;
    // Clear out information about ways
    bld.wayID      = wayID;
    bld.wayLat     = wayLat;
    bld.wayLon     = wayLon;
    bld.pumaID     = pumaID;
    // Return the building
    return bld;
}

void
Building::write(std::ostream& os, const bool writeHeader,
                const std::string& delim) const {
    // Write optional header if requested
    if (writeHeader) {
        os << "# Bld ID"  << delim << "levels" << delim << "population" << delim
           << "attributes"<< delim << "isHome" << delim << "sqFoot"     << delim
           << "topLon"    << delim << "topLat" << delim << "botLon"     << delim
           << "botLat"    << delim << "wayID"  << delim << "wayLat"     << delim
           << "wayLon"    << delim << "pumaID" << delim << "#households"
           << std::endl;
    }
    // Write the information for this building.
    os << "bld"      << delim;
    os << id         << delim << levels << delim << population << delim
       << attributes << delim << isHome << delim << sqFootage  << delim
       << topLon     << delim << topLat << delim << botLon     << delim
       << botLat     << delim << wayID  << delim << wayLat     << delim
       << wayLon     << delim << pumaID << delim << households.size()
       << std::endl;
}

void
Building::read(std::istream& is) {
    // Read information for this building
    std::string bld;     // string to be read and discarted
    size_t hldSize = 0;  // Number of households
    is >> bld;
    is >> id     >> levels >> population >> attributes
       >> std::boolalpha   >> isHome     >> sqFootage
       >> topLon >> topLat >> botLon     >> botLat     >> wayID  >> wayLat
       >> wayLon >> pumaID >> hldSize;
}

void
Building::writeHouseholds(std::ostream& os, bool writeHeader,
                          const std::string& delim) const {
    for (const PUMSHousehold& hld : households) {
        hld.write(os, writeHeader);
        writeHeader = false;
    }
}

void
Building::addHousehold(const PUMSHousehold& hld, const int people,
                       const std::string& peopleInfo) {
    population += people;
    households.push_back(PUMSHousehold(hld, id, people, peopleInfo));
}

#endif
