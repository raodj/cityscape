#ifndef SHP_BLD_CATALOG_CPP
#define SHP_BLD_CATALOG_CPP

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

#include <omp.h>
#include "Utilities.h"
#include "ShpBldCatalog.h"

long ShpBldCatalog::getRndBldID(const std::string& areaID) const {
    // We do a thread local here to correctly handle multithreading.
    static thread_local std::default_random_engine rndGen;    

    auto entry = catalog.find(areaID);
    if (entry == catalog.end()) {
        return -1;  // no such area ID in the catalog.
    }
    // Generate a random index based on number of buildings in this area.
    const auto& bldIDList = entry->second;
    std::uniform_int_distribution<> distrib(0, bldIDList.size() - 1);
    return bldIDList.at(distrib(rndGen));
}

void
ShpBldCatalog::buildCatalog(const std::string& shpFilePath,
                            const std::string& dbfFile,
                            const OSMData& model) {
    if (!shpFile.loadShapes(shpFilePath, dbfFile)) {
        std::cerr << "Error loading shape/DBF file: "
                  << shpFilePath << " or "
                  << dbfFile     << std::endl;
        return;
    }

    // Now add each building (using its centroid) to our internal
    // catalog.
    // const std::vector<std::string> MetaFields = {"area_numbe", "geoid10"};
    int assignedCount = 0;  // Number of buildings assigned
    
#pragma omp parallel reduction(+: assignedCount)
    {
        // Each thread adds its buildings to a per-thread catalog
        // first to reduce contention for the actual catalog. These
        // local entries are later on added in 1-shot to the actual
        // catalog at the end of this parallel block.
        CatalogMap localCatalog;
        
        // Total number of thread running in parallel
        const int numThreads = omp_get_num_threads();
        // The i'th thread process's the i'th building entries in the
        // unordered map. For this we need to get an iterator to the
        // i'th entry first and then skip 'numThreads' entries
        auto bldIter = model.buildingMap.cbegin();    // Current building
        auto bldEnd  = model.buildingMap.cend();      // End of buildings
        int skipEntries = omp_get_thread_num();       // Entries to skip
        int prevRing = -1;   // Previous successful ring match
        
        // Process builidings.
        while (bldIter != bldEnd) {
            // Move iterator to the n'th entry. In the first iteration
            // it skips to the n'th entry. Subsequent iterations it skips 
            for (int i = 0; (i < skipEntries) && (bldIter != bldEnd);
                 i++, bldIter++) {}
            // Next time we will skip numThread entries
            skipEntries = numThreads;

            if (bldIter == bldEnd) {
                break;  // All builidings processed.
            }

            // Try and assign building to a ring.
            const Building& bld = bldIter->second;
            int assignToRing = -1;  // ring to assign building to
            // Buildings are typically adjacent to each other and
            // hence checking the previous ring first eliminates
            // unnecessary processing making things a bit faster.            
            if (prevRing != -1 &&
                shpFile.getRing(prevRing).contains(bld.wayLon, bld.wayLat)) {
                assignToRing = prevRing;
            } else {
                // The previous ring did not work. We need to check
                // all the rings.
                prevRing = -1;
                for (int i = 0; (i < shpFile.getRingCount()); i++) {
                    if (shpFile.getRing(i).contains(bld.wayLon, bld.wayLat)) {
                        // Found a ring that contains the building.
                        assignToRing = prevRing = i;
                        break;
                    }
                }
            }

            // If we found a ring that contains the building then we
            // update our local catalog.
            if (assignToRing != -1) {
                // Found a ring that contains the building.
                const Ring& ring = shpFile.getRing(assignToRing);
                const std::string ringID = ring.getInfo("geoid10");
                ASSERT(!ringID.empty());
                localCatalog[ringID].push_back(bld.id);
                assignedCount++;  // track buildings assigned.
                prevRing = assignToRing;
            }
        }

        // Now that we have processed all the buildings it is time to
        // update the global catalog.
#pragma omp critical (buildCatalog)
        {
            // Add all local entries to the main catalog.
            for (const auto& entry : localCatalog) {
                catalog[entry.first].insert(catalog[entry.first].end(),
                                            entry.second.begin(),
                                            entry.second.end());
            }
        }
        
    }  // OMP parallel block

    std::cout << "Assigned " << assignedCount << " buildings out of "
              << model.buildingMap.size() << " buildings in model, "
              << "in the following manner:\n";
    for (const auto& entry : catalog) {
        std::cout << "    Census tract " << entry.first << ": "
                  << entry.second.size() << " buildings.\n";
    }

    /*
      
    // Some debugging code left in place as partial solution of how
    // the unassigned_buildings.fig file was originally generated in
    // single-threaded mode.
    if (!assigned) {
        std::cout << "Unable to find shape for building "
                  << bld.id << " lat = " << bld.wayLat
                  << ", lon = " << bld.wayLon << ") from "
                  << shpFile.getRings().size() << " rings.\n";
        Ring bldRng(Point(bld.topLon, bld.topLat),
                    Point(bld.botLon, bld.botLat), bld.population);
        bldRng.setKind(Ring::BUILDING_RING);
        shpFile.addRing(bldRng);
    }

    shpFile.genXFig("test.fig", 1638400, false);
    */
}

#endif
