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
#include <numeric>
#include <stdexcept>
#include "PUMS.h"
#include "Utilities.h"
#include "PUMSPerson.h"

int
PUMS::loadPUMA(const std::string& pumaShpPath, const std::string& pumaDbfPath,
               const double minX, const double minY, const double maxX,
               const double maxY, const ShapeFile& communities,
               const double roundUpThreshold) {
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
        const std::string label = ring.getInfo("PUMA") +
            ring.getInfo("PUMACE10"); 
        const int pumaID        = std::stoi(label);
        // Handle 2 cases -- 1: fully contained, 2: partial intersection
        if (bounds.contains(ring)) {
            // Get finer area with intersecting shapes
            double fracPop = getIntersectionOverlap(ring, communities, 1);
            if (fracPop > 0) {
                // The PUMA ring is fully contained in our bounds!
                Ring copy = Ring(ring);   // Create copy to update attributes
                copy.setPopulation((fracPop >= roundUpThreshold ? 1 : fracPop));  // Include part of population
                // Setup the ID for the ring to the be same as the PUMA id.
                copy.setLabel(label);
                copy.setRingID(pumaID);
                copy.setKind(Ring::PUMA_RING);
#pragma omp critical(PUMA_CS)
                puma.addRing(copy);  // Add copy to our common PUMA list.
            }
        } else if (bounds.intersects(ring)) {
            // The PUMA ring is not fully contained but only intersects
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
            double fracPop =
                getIntersectionOverlap(ring, communities,
                                       inter.getArea() / ring.getArea());
            // Round up fractional population to handle cases in 2020
            // PUMA where the PUMA regions have been made bigger than
            // the communities.  This is the case along the lake for
            // Chicago.
            if (fracPop >= roundUpThreshold) {
                fracPop = 1.0;
            }
            inter.setPopulation(fracPop);
            if (fracPop > 0) {
#pragma omp critical(PUMA_CS)            
                puma.addRing(inter);   // Add intersection to our PUMA list.
            }
        }
    }  // parallel for

    // Draw a figure to show the PUMA regions we are working with.
    // puma.genXFig("puma.fig", 1638400);

    return 0;  // Everything went well
}

// Internal method to compute finer overlapping areas between a given
// PUMA ring and community shapes (if available)
double
PUMS::getIntersectionOverlap(const Ring& pumaRng, const ShapeFile& communities,
                             const double defOverlap) const {
    if (communities.getRingCount() == 0) {
        return defOverlap;  // No community rings. Use default overlap
    }
    // Check overlap with community shapes and accummulate percentage
    // overlaps between multiple communities.
    const double pumaArea = pumaRng.getArea();
    double overlap = 0;
    for (const Ring& commRng : communities.getRings()) {
        if (pumaRng.intersects(commRng)) {
            // Here there is full/partial intersection.
            Ring inter = pumaRng.intersection(commRng);
            overlap += inter.getArea() / pumaArea;
        }
    }
    return std::min(1.0, overlap);
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

int
PUMS::findPUMAIndex(const int pumaID) const {
    for (int i = 0; (i < puma.getRingCount()); i++) {
        if (puma.getRing(i).getRingID() == pumaID) {
            return i;  // found the pumaID
        }
    }
    return -1;  // PUMA ID not found.
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
        distributePopulation(ring.getRingID(), ring.getPopulation(),
                             buildings, households);
    }
}

// Load household information into the map
HouseholdMap
PUMS::loadHousehold(const std::string& pumsHouPath,
                    const ArgParser::StringList& pumsColNames) const {
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
    constexpr int SERIALNO = 1, PUMA = 3, WGTP = 8, NP = 9, TYPE = 10,
        BDSP = 15, BLD = 16, HINCP = 73; // HINCP = 67;

    // Double check order of columns just to play it safe if program
    // is compiled in development mode.
#ifdef DEVELOPER_ASSERTIONS
    const std::vector<std::string> titleCols = split(header, ",");
    ASSERT((titleCols[SERIALNO]  == "SERIALNO") &&
           (titleCols[PUMA]  == "PUMA")         &&
           (titleCols[WGTP]  == "WGTP")         &&
           (titleCols[NP]    == "NP")           &&
           (titleCols[BDSP]  == "BDSP")         &&
           (titleCols[BLD]   == "BLD")          &&
           (titleCols[HINCP] == "HINCP"));
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
        // not group quarters. Note: We ignore mobile homes.
        if ((info[TYPE] == "1") && (info[BLD] != "01")) {
            // For some households the income HINCP is empty. Use -1 for them.
            const int hincp = (info[HINCP].empty() ? -1 :
                               std::stoi(info[HINCP]));
            // Create a template household. This template has people
            // == -1. Actual households are created in
            // distributePopulation method.
            PUMSHousehold ph(houseInfo, std::stoi(info[BDSP]),
                             std::stoi(info[BLD]), std::stoi(info[PUMA]),
                             std::stoi(info[WGTP]), hincp, std::stoi(info[NP]));
            ASSERT(ph.getBld() != 1);
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
        if ((index >= 0) && (index < static_cast<int>(titleCols.size()))) {
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
    const std::vector<std::string> titleCols = split(header, ",");    
#ifdef DEVELOPER_ASSERTIONS
    ASSERT((titleCols[SERIALNO]  == "SERIALNO") &&
           (titleCols[PUMA]   == "PUMA")        &&
           (titleCols[PWGTP]  == "PWGTP"));
#endif
    // Setup the fixed column names in the person's entry
    PUMSPerson::setColumnTitles(titleCols, colIndexs);
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
        const PUMSPerson personInfo(serialNo, colIndexs, info);
        const int pwgtp = std::stoi(info[PWGTP]);
        // Add the person to the household
        entry->second.addPersonInfo(pwgtp, personInfo);
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
            bldSizeList.push_back({i, buildings[i].getArea()});
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
        if (entry.second.getPUMAId() == pumaID) {
            const size_t houSz = entry.second.getSize();
            houIdSzList.push_back(HouIdSize(entry.first, houSz));
        }
    }
    // Sort of household size
    std::sort(houIdSzList.begin(), houIdSzList.end(),
              [](const HouIdSize& h1, const HouIdSize& h2)
              { return h1.second < h2.second; });
    // Return the sorted list back
    return houIdSzList;
}

#pragma GCC push_options
#pragma GCC optimize ("-O0")

// NOTE: This method is called from multiple threads.
void
PUMS::distributePopulation(int pumaID, const double popFrac,
                           std::vector<Building>& buildings,
                           HouseholdMap& households) const {
    // Get the list of buildings in the area, sorted on sq.foot
    const std::vector<BldIdxSqFt> bldSzList =
        getSortedBldList(pumaID, buildings);
    // Get the list of households in the area, sorted on house size
    const std::vector<HouIdSize> houSzList =
        getSortedHouList(pumaID, households);
    // Ignore PUMA areas that have no buildings or households
    if (bldSzList.empty() || houSzList.empty()) {
        std::cout << "PUMA ID " << pumaID << " ignored. (buildings = "
                  << bldSzList.size() << ", households = "
                  << houSzList.size() << std::endl;
        return;
    }
    std::cout << "Distributing population for pumaID " << pumaID
              << " with popFrac = " << popFrac << ", using "
              << (houSzList.size() * popFrac)  << " hld templates from "
              << houSzList.size() << " number of templates and "
              << bldSzList.size() << " buildings.\n";
    // Now we have 2 lists of sorted buildings and households. For
    // each household start assigning buildings.
    size_t currBld = 0, currHld = 0;  // building & household counter
    // First assign households that live in single-family homes.
    int hldsCreated = assignSingleFamilies(buildings, households, bldSzList,
                                           houSzList, popFrac, currHld,
                                           currBld, pumaID);

    // Next we assign apartment buildings that hold multiple
    // households.  Households are assigned based on number of
    // bedrooms for each family and square footage of buildings.  For
    // this we need to estimate average sq.ft-per-bedroom.
    const int totRooms =
        std::accumulate(houSzList.begin() + currHld, houSzList.end(), 0,
                        [&households](const int& rooms, const HouIdSize& ent) {
                            const PUMSHousehold& hld = households.at(ent.first);
                            return rooms + (hld.getRooms() * hld.getCount());
                        });
    const int totSqFt =
        std::accumulate(bldSzList.begin() + currBld, bldSzList.end(), 0,
                        [&buildings](const int& sqFt, const BldIdxSqFt& bld) {
                            return sqFt + buildings.at(bld.first).getArea();
                        });
    const int sqFtPerRm = totSqFt / (totRooms * popFrac);
    std::cout << "For puma " << pumaID << ", average sq.ft/room = "
              << sqFtPerRm << " from " << (bldSzList.size() - currBld)
              << " buildings and " << (houSzList.size() - currHld)
              << " household rows, totalRooms = " << totRooms << std::endl;

    if ((currBld >= bldSzList.size()) || (currHld >= houSzList.size())) {
        // No need to do any further processing as we are either out of
        // buildings or households.
        return;
    }
    // Assign households to buildings.  Recollect that at this point
    // the households and buildings are sorted.
    int remainingSqFt = buildings.at(bldSzList[currBld].first).getArea();
    const std::string hldId = houSzList.at(currHld).first;
    int hldsRemaining = households.at(hldId).getCount() * popFrac;
    if (pumaID == 3520) {
        std::cout << "Households: " << pumaID << ", "
                  << households.at(hldId).getCount() << ", "
                  << households.at(hldId).getPeopleCount() << std::endl;
    }
    
    // Keep assigning buildings.
    while ((currBld < bldSzList.size()) && (currHld < houSzList.size())) {
        // Check and skip to next building if we have run out of space
        // in the current building.
        if (remainingSqFt <= 0) {
            currBld++;
            if (currBld < bldSzList.size()) {
                remainingSqFt = buildings.at(bldSzList[currBld].first).getArea();
            }
            continue;
        }
        // Move onto the next type of household if we have assigned
        // all households in the current type of household.
        if (hldsRemaining < 1) {
            currHld++;
            if (currHld < houSzList.size()) {
                const std::string hldId = houSzList.at(currHld).first;
                hldsRemaining = households.at(hldId).getCount() * popFrac;
                if (pumaID == 3520) {
                    std::cout << "Households: " << pumaID << ", "
                              << households.at(hldId).getCount() << ", "
                              << households.at(hldId).getPeopleCount()
                              << std::endl;
                }
            }
            continue;
        }

        // Assign current household to current building
        int pepCount = -2;
        const std::string hldId = houSzList.at(currHld).first;
        const std::vector<PUMSPerson> hldInfo =
            households.at(hldId).getInfo(hldsRemaining, pepCount);
        hldsRemaining--;  // Household processed.
        if (hldInfo.empty()) {
            continue;  // In some fractional cases we skip an household
        }
        
        // Assign household to the current building.
        // Have a helper method assign the i'th familiy to the
        // current building.
        const size_t bldMainIdx = bldSzList.at(currBld).first;
        ASSERT((bldMainIdx >= 0) && (bldMainIdx < buildings.size()));
        Building& bld = buildings[bldMainIdx];
        // Add household to building with given people count.
        const PUMSHousehold& hld = households.at(hldId);
        bld.addHousehold(hld, pepCount, hldInfo, true);
        hldsCreated++;
        // Decrease area left in this building
        remainingSqFt -= (hld.getRooms() * sqFtPerRm);
    }

    std::cout << "distributePopulation: For PUMA ID " << pumaID
              << " assigned " << hldsCreated
              << " households using " << currHld << " templates  of "
              << houSzList.size() << " to " << currBld
              << " buildings out of " << bldSzList.size() << " buildings\n";
}

// Method to assign single-family households to buildings
int
PUMS::assignSingleFamilies(std::vector<Building>& buildings,
                           HouseholdMap& households,
                           const std::vector<BldIdxSqFt> bldSzList,
                           const std::vector<HouIdSize> houSzList,
                           const double popFrac,
                           size_t& hldIdx, size_t& bldIdx,
                           const int pumaID) const {
    // A shortcut to number of buildings to streamline code below
    const size_t BldCount = bldSzList.size();
    const size_t startBldIdx = bldIdx;  // just to print stats below
    const size_t startHldIdx = hldIdx;  // just to print stats below
    // Keep assigning single-family households
    int hldsAssigned = 0;
    while ((hldIdx < houSzList.size()) && (bldIdx < bldSzList.size())) {
        // Get the household information that we are working with
        const PUMSHousehold& hld = households.at(houSzList.at(hldIdx).first);
        // If the household we are working with is no longer a single
        // family household then break out of this loop
        if (hld.getBld() > 3) {
            break;  // End of single family homes.
        }

        if ((hld.getBld() != 2) && (hld.getBld() != 3)) {
            std::cout << "For pumaID " << pumaID << " encountered incorrect "
                      << "building type: " << hld.getBld() << " for "
                      << houSzList.at(hldIdx).first << std::endl;
            continue;  // This is not an attached/detached home.
        }

        // Each household has 1-or-more occurrences associated with
        // them. So we need to assign with multiple families.
        const int hldCount = hld.getCount() * popFrac;
        if (pumaID == 3520) {
            std::cout << "Households: " << pumaID << ", " << hldCount
                      << ", " << hld.getPeopleCount() << std::endl;
        }
        for (int i = 0; ((i < hldCount) && (bldIdx < BldCount)); i++) {
            if (hld.getPeopleCount() < 1) {
                continue;  // Some households have zero people in PUMS
            }
            // Get the i'th household information
            int pepCount;
            const std::vector<PUMSPerson> hldInfo = hld.getInfo(i, pepCount);
            if (hldInfo.empty()) {
                std::cout << "For pumaID " << pumaID << " got empty info for "
                          << houSzList.at(hldIdx).first << " even if it has "
                          << hld.getPeopleCount() << " people\n";
                continue;
            }
            
            // Have a helper method assign the i'th familiy to the
            // current building.
            const size_t bldMainIdx = bldSzList.at(bldIdx).first;
            ASSERT((bldMainIdx >= 0) && (bldMainIdx < buildings.size()));
            Building& bld = buildings[bldMainIdx];
            // Basic checks for compatibility between buildings and
            // households.
            const int sqFtPerRoom = bld.getArea() / hld.getRooms();
            if ((sqFtPerRoom < 100) || (sqFtPerRoom > 2000)) {
                std::cout << "Building "     << bld.id << " is bad size for "
                          << "household "    << houSzList.at(hldIdx).first
                          << ", bld type "   << hld.getBld()
                          << ", rooms = "    << hld.getRooms()
                          << ", sqFt/room = " << sqFtPerRoom
                          << " (puma ID = "   << pumaID << ")\n";
            }
            // Add household to building
            bld.addHousehold(hld, pepCount, hldInfo, true);
            hldsAssigned++;
            // Onto the next building.
            bldIdx++;
        }
        // If we run out of the buildings we break out of this loop
        if (bldIdx >= BldCount) {
            std::cout << "Ran out of buildings for PUMA id " << pumaID
                      << ". Bld count = " << BldCount << ".\n";
            break;  // Break of out the outer range-based for-loop.
        }
        // On to the next single-family household (if any)
        hldIdx++;
    }
    // Print stats on number of households assigned
    std::cout << "For PUMA " << pumaID << ": distributed " << hldsAssigned
              << " single-family households to "  << (bldIdx - startBldIdx)
              << " buildings out of " << BldCount << " buildings using "
              << (hldIdx - startHldIdx) << " of " << houSzList.size()
              << " PUMS records.\n";
    return hldsAssigned;
}

#pragma GCC pop_options

#endif
