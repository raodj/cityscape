#ifndef RADIUS_FILTER_WORK_BUILDING_ASSIGNER_CPP
#define RADIUS_FILTER_WORK_BUILDING_ASSIGNER_CPP

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

#include "Utilities.h"
#include "OSMData.h"
#include "PathFinder.h"
#include "RadiusFilterWorkBuildingAssigner.h"

void
RadiusFilterWorkBuildingAssigner::assignWorkBuilding() {
    std::cout << "Generating Schedule..." << std::endl;
    // Classify buildings by their types in order to reduce the number
    // of buildings we iterate on for different operations.
    auto [homeBuildings, nonHomeBuildings] =
        getHomeAndNonHomeBuildings(model.buildingMap);
    
    for (auto& [bldId, bld] : homeBuildings) {
        if (bld.households.empty()) {
            continue;  // no households in this home
        }
        // For each person in each household assign work building
        for (auto& hld : bld.households) {
            for (auto& ppl : hld.getPeopleInfo()) {
                if (ppl.getIntegerInfo(jwtrnsIdx) != 1) {
                    continue;  // This person doesn't drive to work.
                }
                const int travelTime = ppl.getIntegerInfo(jwmnpIdx);
                if (travelTime < 0) {
                    continue;  // This person doesn't travel for work
                }
                // Find buildings in the
                const int timeMargin = 3;  // Wiggle room of 3 minutes
                for (int slack = 0; slack < timeMargin; slack++) {
                    BuildingList candidateWorkBlds =
                        getCandidateWorkBuildings(bld, nonHomeBuildings,
                                                  travelTime, travelTime, slack);
                    if (!candidateWorkBlds.empty()) {
                        assignWorkBuildings(model, bld, nonHomeBuildings,
                                            candidateWorkBlds,
                                            ppl, timeMargin);
                    }
                }
            } // for each household
        }
    }
}

std::pair<BuildingMap, BuildingMap>
RadiusFilterWorkBuildingAssigner::getHomeAndNonHomeBuildings(const BuildingMap& buildingMap) const {
    BuildingMap homeBuildings, nonHomeBuildings;
    for (auto item = buildingMap.begin(); item != buildingMap.end(); item++) {
        if (item->second.isHome) {
            homeBuildings[item->first] = item->second;
        } else {
            Building bld = item->second;
            // Initialize capacity of this office building based on
            // its square footage.
            bld.population = bld.getArea() / offSqFtPer;
            nonHomeBuildings[bld.id] = bld;
        }
    }
    
    ASSERT(homeBuildings.size() + nonHomeBuildings.size() ==
           buildingMap.size());
    
    std::cout << "# of non-home buildings: " << nonHomeBuildings.size() << '\n';
    std::cout << "# of home buildings: " << homeBuildings.size() << std::endl;

    return {homeBuildings, nonHomeBuildings};
}

// Return a list of potential non-home buildings to where people may
// travel between the specified minTravelTime - timeMargin and
// maxTravelTime + timeMargin.
BuildingList
RadiusFilterWorkBuildingAssigner::
getCandidateWorkBuildings(const Building& srcBld,
                          const BuildingMap& nonHomeBlds,
                          const int minTravelTime,
                          const int maxTravelTime,
                          const int timeMargin) const {
    // There are some work travel times that are pretty large which do
    // not approximate well.  In this situation, we track the building
    // with maximum distance and use that building as the fall back.
    Building maxDistBld = srcBld;
    double maxDist = 0;
    
    // The list of potential candidates.
    BuildingList candidateBlds;

    for (const  auto& [bldId, bld] : nonHomeBlds) {
        // Get the distance in miles from the source building to an
        // non-office building.
        const double dist = getDistance(srcBld.wayLat, srcBld.wayLon,
                                        bld.wayLat,    bld.wayLon);
        // Track maximum distance building
        if (dist > maxDist) {
            maxDistBld = bld;
            maxDist    = dist;
        }
        // Convert distance to time using an "average" estimated time.
        // This approach is used because computing actual path is
        // slower.
        const int timeInMinutes = std::round(dist * 60 / avgSpeed);

        // Use only buildings that are within our specified time ranges
        if ((timeInMinutes >= (minTravelTime - timeMargin)) &&
            (timeInMinutes <= (maxTravelTime + timeMargin)) &&
            (bld.population > 0)) {
            candidateBlds.push_back(bld);
        }
    }

    // Sort the candidate non-home buildings based on their distances
    // to current building (approximating travel time)
    // std::sort(candidateBlds.begin(), candidateBlds.end(),
    //           [&srcBld](const Building& b1, const Building& b2) {
    //               return getDistance(srcBld.wayLat, srcBld.wayLon,
    //                                  b1.wayLat, b1.wayLon) > 
    //                   getDistance(srcBld.wayLat, srcBld.wayLon,
    //                               b2.wayLat, b2.wayLon);
    //           });

    std::cout << "# of candidate work locations: " << candidateBlds.size()
              << std::endl;
    if (candidateBlds.empty()) {
        std::cout << "Unable to find candidate buildings for time: "
                  << minTravelTime << " for building: " << srcBld.id
                  << ". Using max distance building: " << maxDistBld.id
                  << ", at a distance of: " << maxDist << std::endl;
        candidateBlds.push_back(maxDistBld);
        /*
        for (const  auto& [bldId, bld] : nonHomeBlds) {
            // Get the distance in miles from the source building to an
            // non-office building.
            const double dist = getDistance(srcBld.wayLat, srcBld.wayLon,
                                            bld.wayLat,    bld.wayLon);
            // Convert distance to time using an "average" estimated time.
            // This approach is used because computing actual path is
            // slower.
            const int timeInMinutes =
                std::round(dist * 60 / cmdLineArgs.avgSpeed);
            std::cout << "    Building: " << bldId << ", dist: " << dist
                      << ", time: " << timeInMinutes << ".\n";
        }
        */
    }
    return candidateBlds;
}

std::unordered_map<long, long>
RadiusFilterWorkBuildingAssigner::assignWorkBuildings(const OSMData& model,
                                       const Building& bld,
                                       BuildingMap& nonHomeBuildings,
                                       BuildingList& candidateWorkBlds,
                                       const PUMSPerson& person,
                                       const int timeMargin) {
    // To speed up the assignment, a building will be assigned to
    // a person if the travel time to that building is between
    // travelTime - timeMargin and travelTime for that person
    int bldCount = 0;
    const int travelTime = person.getIntegerInfo(jwmnpIdx);
    for (Building& wrkBld : candidateWorkBlds) {
        if (wrkBld.population < 1) {
            continue;  // No space left in work building
        }
        bldCount++;
        PathFinder pf(model);
        const Path path = pf.findBestPath(bld.id, wrkBld.id,
                                          true, 0.25, 0.1);
        if (path.size() == 0) {
            continue;
        }
        int timeInMinutes = (int)(std::round(path.back().distance * 60));
        if (std::abs(travelTime - timeInMinutes) > timeMargin) {
            std::cout << "    Path time is " << timeInMinutes << ", expected: "
                      << travelTime << std::endl;
            continue;  // This building is not acceptable
        }

        // Found a matching building
        wrkBld.population--;
        // Also update the actual building entry
        nonHomeBuildings[wrkBld.id].population--;
        std::cout << "    Assign Building " << wrkBld.id << "(in ring: "
                  << wrkBld.attributes << ") to person " << person.getPerID()
                  << " in building " << bld.id << "(in ring: " << bld.attributes
                  << ") after checking " << bldCount << " buildings\n";
        std::cout << "    Person needs time: " << travelTime
                  << " and the assigned building has time: "
                  << timeInMinutes << std::endl;
        return {{person.getPerID(), wrkBld.id}};
    }
    std::cerr << "Unable to find suitable work building for person ";
    person.write(std::cerr);
    return {};
}

#endif
