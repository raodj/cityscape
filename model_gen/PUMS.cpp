#ifndef PUMS_CPP
#define PUMS_CPP

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

#include <algorithm>
#include <stdexcept>
#include "PUMS.h"
#include "Utilities.h"

int
PUMS::loadPUMA(const std::string& pumaShpPath, const std::string& pumaDbfPath,
               const double minX, const double minY, const double maxX,
               const double maxY) {
    // First load the PUMA shape file and retain only the rings that
    // we care about.
    ShapeFile fullPuma;
    if (!fullPuma.loadShapes(pumaShpPath, pumaDbfPath, 1.0, true, true)) {
        std::cerr << "Error loading PUMA data.\n";
        return 2;  // Error
    }
    std::cout << "Loaded " << fullPuma.getRingCount() << " PUMA rings.\n"
              << "Filtering out unused PUMA rings to minimize overheads...\n";
    
    // Filter out the rings that we care about based on the bounds
    // provided.  We create a bounds ring (in clockwise direction) to
    // streamline comparisons.
    const Ring bounds(Point(minX, maxY), Point(maxX, minY), -1);
    // Check each ring in the full PUMA data and add the rings we care
    // about to the the PUMA instance variable.

#pragma omp parallel for schedule(guided)
    for (int rIdx = 0; (rIdx < fullPuma.getRingCount()); rIdx++) {
        // Get the ring that we are working with
        const Ring& ring = fullPuma.getRing(rIdx);
        // Get the PUMA area ID to set as the ID for the ring to make
        // future look-up/validation/troubleshooting easier.
        const std::string label = ring.getInfo("PUMA");        
        const int pumaID        = std::stoi(label);
        // Handle 2 cases -- 1: fully contained, 2: partial intersection
        if (bounds.contains(ring)) {
            // The PUMA ring is fully contained in our bounds!
            Ring copy = Ring(ring);   // Create copy to update attributes
            copy.setPopulation(1.0);  // Include full population
            // Setup the ID for the ring to the be same as the PUMA id.
            copy.setLabel(label);
            copy.setRingID(pumaID);
            copy.setKind(Ring::PUMA_RING);
#pragma omp critical (PUMA_CS)
            puma.addRing(copy);  // Add copy to our common PUMA list.
        } else if (bounds.intersects(ring)) {
            // The PUMA ring is not fully contained by only intersects
            // with our bounds. So create a partial intersecting
            // polygon for this PUMA ring.
            Ring inter =
                bounds.intersection(ring, Ring::PUMA_RING, ring.getRingID(),
                                    ring.getShapeID(), 1.0, ring.getInfoList());
            // Setup other attributes
            inter.setLabel(label);
            inter.setRingID(pumaID);            
            // Set the pouplation of the intersection ring to be the
            // appropriate fraction of the population of the
            // intersection area.
            const double fracPop = inter.getArea() / ring.getArea();
            inter.setPopulation(fracPop);
#pragma omp critical (PUMA_CS)            
            puma.addRing(inter);   // Add intersection to our common PUMA list.
        }
    }

    // Draw a figure to show the PUMA regions we are working with.
    puma.genXFig("puma.fig", 1638400);

    return 0;  // Everything went well
}

std::string
PUMS::getPUMAIds() const {
    std::string ids;
    // Get the list of Ring IDs into a string.
    for (const Ring& ring : puma.getRings()) {
        ids += " ";
        ids += std::to_string(ring.getRingID());
    }
    // Return the list of IDs
    return ids;
}

int
PUMS::findPUMAIndex(const Point& vertex, const int startPumaIndex) const {
    const int ringCount = puma.getRingCount();  // number of rings    
    int ringIndex = std::min(ringCount, std::max(0, startPumaIndex));
    const int startIndex = ringIndex;   // remember where we really started
    // Check each puma ring to see if the given point lies within the ring.
    do {
        if (puma.getRing(ringIndex).contains(vertex.first, vertex.second)) {
            return ringIndex;   // found a ring that contains the vertex
        }
        // Onto the next PUMA ring to check.
        ringIndex = (ringIndex + 1) % ringCount;
    } while (ringIndex != startIndex);

    // When control drops here, none of the rings contain the given
    // building.
    return -1;
}

// Generates xfig file with pums data along with extra rings (if any)
void
PUMS::drawPUMA(const ShapeFile& extraRings, const std::string& xfigPath,
               const int scale) const {
    // Make a copy of the PUMA shapes
    ShapeFile regions(puma);
    // Add the supplied extra rings
    regions.addRings(extraRings.getRings());
    // Draw the xfig
    regions.genXFig(xfigPath, scale);
}

// Top-level method to distribute houesholds to buildings
void
PUMS::distributePopulation(std::vector<Building>& buildings,
                           const std::string& pumsHouPath,
                           const std::string& pumsPepPath,
                           const ArgParser::StringList& pumsHouColNames,
                           const ArgParser::StringList& pumsPepColNames) const {
    // First load the household information for rings of interest into
    // memory.
    HouseholdMap households = loadHousehold(pumsHouPath, pumsHouColNames);
    // Load the people information from the PUMS file into the
    // household hashmap (created in the previous line of code).
    loadPeopleInfo(households, pumsPepPath, pumsPepColNames);
    // Now, we have loaded the information into memory, we can
    // distribute households to the different buildings in the
    // different pums regions.
    const int MaxRings = puma.getRingCount();
    for (int i = 0; (i < MaxRings); i++) {
        const Ring& ring = puma.getRing(i);
        distributePopulation(ring.getRingID(), buildings, households);
    }
}

// Load household information into the map
HouseholdMap
PUMS::loadHousehold(const std::string& pumsHouPath,
                    const ArgParser::StringList& pumsColNames) const{
    std::ifstream houCsv(pumsHouPath);  // Open the CSV file.
    if (!houCsv.good()) {
        std::string msg = "Error reading PUMS household file " + pumsHouPath;
        throw std::runtime_error(msg);
    }

    // Get the index of column names to retain from the first header
    // line of the file.
    std::string header, line;
    std::getline(houCsv, header);
    const std::vector<int> colIndexs = toColIndex(header, pumsColNames);
    // Validate implicit assumptions on column order below.
    constexpr int SERIALNO = 1, PUMA = 3, WGTP = 8, TYPE = 10,
        BDSP = 15, BLD = 16;

    // Double check order of columns just to play it safe if program
    // is compiled in development mode.
#ifdef DEVELOPER_ASSERTIONS
    const std::vector<std::string> titleCols = split(header, ",");
    ASSERT((titleCols[SERIALNO]  == "SERIALNO") &&
           (titleCols[PUMA]  == "PUMA")         &&
           (titleCols[WGTP]  == "WGTP")         &&
           (titleCols[BDSP]  == "BDSP")         &&
           (titleCols[BLD]   == "BLD"));
#endif
    
    // Process each line of the input and retain the necessary
    // household information.
    HouseholdMap households;    
    while (std::getline(houCsv, line)) {
        // Split into individual columns for convenience.
        const std::vector<std::string> info = split(line, ",");
        // Extract some of the specific household information.
        const std::string houseInfo = toCSV(colIndexs, info);
        // Create the household record to be stored only for homes and
        // not group quarters.
        if (info[TYPE] == "1") {
            PUMSHousehold ph(houseInfo, std::stoi(info[BDSP]),
                             std::stoi(info[BLD]), std::stoi(info[PUMA]),
                             std::stoi(info[WGTP]));
            // Add the household record to our map
            households[info[SERIALNO]] = ph;
        }
    }
    // Return map with household information
    return households;
}

// Compute the index values at specific index positions.
std::vector<int>
PUMS::toColIndex(const std::string& titles,
                 const ArgParser::StringList& pumsColNames) const {
    // Split the title line into column names to make checks easier.
    const std::vector<std::string> titleCols = split(titles, ",");
    // For each pumsColName find the desired index position
    std::vector<int> colIndexs;
    for (const std::string& colName : pumsColNames) {
        const int index =
            std::distance(titleCols.begin(),
                          std::find(titleCols.begin(), titleCols.end(),
                                    colName));
        if ((index >= 0) && (index < (int) titleCols.size())) {
            colIndexs.push_back(index);
        } else {
            std::cerr << "Warning: Unable to find PUMS col with name "
                      << colName << std::endl;
        }
    }
    return colIndexs;
}

// Combines values at specific indexes into a CSV
std::string
PUMS::toCSV(const std::vector<int>& colIndexs,
            const std::vector<std::string>& info) const {
    std::string csv;  // the string to be built
    // Combine the info to make a string
    for (int idx : colIndexs) {
        csv += info.at(idx) + ",";
    }
    // Remove trailing comma
    csv.pop_back();
    // Return the string.
    return csv;
}

void
PUMS::loadPeopleInfo(HouseholdMap& households,
                     const std::string& pumsPepPath,
                     const ArgParser::StringList& pumsColNames) const {
    std::ifstream pepCsv(pumsPepPath);  // Open the CSV file.
    if (!pepCsv.good()) {
        std::string msg = "Error reading PUMS people file " + pumsPepPath;
        throw std::runtime_error(msg);
    }

    // Get the index of column names to retain from the first header
    // line of the file.
    std::string header, line;
    std::getline(pepCsv, header);
    const std::vector<int> colIndexs = toColIndex(header, pumsColNames);
    // Validate implicit assumptions on column order below.
    constexpr int SERIALNO = 1, PUMA = 4, PWGTP = 8;

    // Double check order of columns just to play it safe if program
    // is compiled in development mode.
#ifdef DEVELOPER_ASSERTIONS
    const std::vector<std::string> titleCols = split(header, ",");
    ASSERT((titleCols[SERIALNO]  == "SERIALNO") &&
           (titleCols[PUMA]   == "PUMA")        &&
           (titleCols[PWGTP]  == "PWGTP"));
#endif
    
    // Process each line of the input and retain the necessary
    // household information.
    while (std::getline(pepCsv, line)) {
        // Split into individual columns for convenience.
        const std::vector<std::string> info = split(line, ",");
        // Extract some of the specific household information.
        const std::string& serialNo = info[SERIALNO];
        // Create the household record to be stored only for homes and
        // not group quarters.
        HouseholdMap::iterator entry = households.find(serialNo);
        if (entry == households.end()) {
            continue;  // we don't care about this serial number
        }
        // This serial number we care about. Extract necessary columns
        // of people information.
        const std::string peopleInfo = toCSV(colIndexs, info);
        const int pwgtp = std::stoi(info[PWGTP]);
        // Add the person to the household
        entry->second.addPersonInfo(pwgtp, peopleInfo);
    }
}

// ---------------------------------------------------------------
// The household to building assignment logic and helper methods

// Internal helper method to return a sorted building list.
std::vector<PUMS::BldIdxSqFt>
PUMS::getSortedBldList(const int pumaID,
                       const std::vector<Building>& buildings) const {
    // First get a list of homes for the given PUMA region to be sorted on
    // their sq. footage
    std::vector<BldIdxSqFt> bldSizeList;  // Vector to be returned

    // First collect information we need into a vector
    for (size_t i = 0; (i < buildings.size()); i++) {
        if ((buildings[i].isHome) && (buildings[i].pumaID == pumaID)) {
            bldSizeList.push_back({i, buildings[i].sqFootage});
        }
    }
    // Sort on sq. footage to place biggest homes last
    std::sort(bldSizeList.begin(), bldSizeList.end(),
              [](const BldIdxSqFt& b1, const BldIdxSqFt& b2)
              { return b1.second < b2.second; });
    // Return the sorted list
    return bldSizeList;
}

// Internal helper method to return a sorted list of PUMS households
std::vector<PUMS::HouIdSize>
PUMS::getSortedHouList(const int pumaID, const HouseholdMap& households) const {
    // The resulting list to be populated and sorted.
    std::vector<HouIdSize> houIdSzList;

    // First the PUMS household record for the given PUMA region
    // sorted on BLD and then on number of bedrooms.  Here we use a
    // pair with housing ID and (BLD * 100 + BDSP) for sorting.    
    for (const HouseholdMap::value_type& entry : households) {
        const size_t houSz = entry.second.getSize();
        houIdSzList.push_back(HouIdSize(entry.first, houSz));
    }
    // Sort of household size
    std::sort(houIdSzList.begin(), houIdSzList.end(),
              [](const HouIdSize& h1, const HouIdSize& h2)
              { return h1.second < h2.second; });
    // Return the sorted list back
    return houIdSzList;
}

// NOTE: This method is called from multiple threads.
void
PUMS::distributePopulation(int pumaID,
                           std::vector<Building>& buildings,
                           HouseholdMap& households) const {
    // Get the list of buildings in the area, sorted on sq.foot
    const std::vector<BldIdxSqFt> bldSzLize =
        getSortedBldList(pumaID, buildings);
    // Get the list of households in the area, sorted on house size
    const std::vector<HouIdSize> houSzList =
        getSortedHouList(pumaID, households);
    // Now we have 2 lists of sorted buildings and households. For
    // each building assign households.
    
}

#endif
