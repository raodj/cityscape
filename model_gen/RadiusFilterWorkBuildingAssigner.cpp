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

#include <omp.h>
#include <ctime>
#include <unistd.h>
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <tuple>
#include <string>
#include <vector>
#include "Utilities.h"
#include "OSMData.h"
#include "PathFinder.h"
#include "MPIHelper.h"
#include "Stopwatch.h"
#include "RadiusFilterWorkBuildingAssigner.h"

// Definition for the static instance variable
MPI_Win RadiusFilterWorkBuildingAssigner::bldIdxWin;

// Just a named constant to keep some of the MPI calls more readable
constexpr int RANK_0 = 0;

std::string getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    return buffer;
}

// ---------------------------
// Helper struct for samples
// ---------------------------
struct LMSample {
    double time;      // minutes
    double distance;  // miles (haversine)
    long homeID;
    long workID;
};

// ---------------------------
// Private helper: generateLinearModel
//
// This method:
//  - selects random building pairs (home, work)
//  - computes haversine distance for each pair
//  - runs pathfinder to obtain travel time
//  - collects N valid samples
//  - fits OLS regression: distance = intercept + slope * time
//  - prints a single summary and first 20 samples
//  - stores slope/intercept/r2 in the class members
//
// This is intentionally implemented as a private helper method
// and is invoked once at the beginning of assignWorkBuilding().
// ---------------------------
void RadiusFilterWorkBuildingAssigner::generateLinearModel(int lmNumSamples,
                                    const BuildingMap& homeBuildings,
                                    const BuildingMap& nonHomeBuildings,
                                    const std::vector<size_t>& homeBldIdList) {
    // Random engine
    std::random_device rd;
    std::mt19937 gen(rd());

    // Build list of non-home building IDs for random sampling
    std::vector<long> nonHomeIds;
    nonHomeIds.reserve(nonHomeBuildings.size());
    for (const auto& p : nonHomeBuildings) {
        nonHomeIds.push_back(p.first);
    }

    // Quick guards
    if (homeBldIdList.empty() || nonHomeIds.empty()) {
        std::cerr << "Insufficient buildings to generate LM samples." 
                  << std::endl;
        // keep modelSlope/intercept as defaults (0)
        modelSlope = 0.0;
        modelIntercept = 0.0;
        modelR2 = 0.0;
        return;
    }

    std::uniform_int_distribution<size_t> homeDist(0, homeBldIdList.size() - 1);
    std::uniform_int_distribution<size_t> nonHomeDist(0, nonHomeIds.size() - 1);

    std::vector<LMSample> samples;
    samples.reserve(lmNumSamples);

    // Limit attempts to avoid infinite loops if many pairs have no path
    const int maxAttempts = std::max(10000, lmNumSamples * 10);
    int attempts = 0;

    // Collect samples
    while ((int)samples.size() < lmNumSamples && attempts < maxAttempts) {
        attempts++;

        // pick random home and random non-home
        size_t hIdx = homeDist(gen);
        long homeId = homeBldIdList[hIdx];

        size_t nhIdx = nonHomeDist(gen);
        long workId = nonHomeIds[nhIdx];

        // avoid selecting same ID (just in case)
        if (homeId == workId) continue;

        // Get building objects
        auto hbIt = homeBuildings.find(homeId);
        auto wbIt = nonHomeBuildings.find(workId);
        if (hbIt == homeBuildings.end() || wbIt == nonHomeBuildings.end()) 
            continue;

        const Building& hb = hbIt->second;
        const Building& wb = wbIt->second;

        // haversine distance
        double distMiles = getDistance(hb.wayLat, hb.wayLon, 
                                       wb.wayLat, wb.wayLon);

        // use PathFinder to compute actual travel time (slowish)
        PathFinder pf(model);
        const Path path = pf.findBestPath(hb.id, wb.id, true, 0.25, 0.1);
        if (path.size() == 0) {
            // no valid path found; skip
            continue;
        }
        
        // as in other code
        double travelTimeMin = std::round(path.back().distance * 60.0); 

        // Accept sample
        LMSample s;
        s.time = travelTimeMin;
        s.distance = distMiles;
        s.homeID = hb.id;
        s.workID = wb.id;
        samples.push_back(s);
    }

    if (samples.empty()) {
        std::cerr << "Unable to generate LM samples (no valid paths found)." 
                  << std::endl;
        modelSlope = 0.0;
        modelIntercept = 0.0;
        modelR2 = 0.0;
        return;
    }

    // ---------------------------
    // Prepare vectors for regression (time -> distance)
    // ---------------------------
    std::vector<double> x_vals; x_vals.reserve(samples.size());  // time
    std::vector<double> y_vals; y_vals.reserve(samples.size());  // distance

    for (const auto &s : samples) {
        x_vals.push_back(s.time);
        y_vals.push_back(s.distance);
    }

    // ---------------------------
    // Compute means
    // ---------------------------
    double sum_x = std::accumulate(x_vals.begin(), x_vals.end(), 0.0);
    double sum_y = std::accumulate(y_vals.begin(), y_vals.end(), 0.0);
    double mean_x = sum_x / x_vals.size();
    double mean_y = sum_y / y_vals.size();

    // ---------------------------
    // Compute slope (β1) and intercept (β0)
    // ---------------------------
    double num = 0.0, den = 0.0;
    for (size_t i = 0; i < x_vals.size(); ++i) {
        num += (x_vals[i] - mean_x) * (y_vals[i] - mean_y);
        den += (x_vals[i] - mean_x) * (x_vals[i] - mean_x);
    }

    double slope = 0.0;
    double intercept = 0.0;
    if (den != 0.0) {
        slope = num / den;
        intercept = mean_y - slope * mean_x;
    } else {
        // degenerate case
        slope = 0.0;
        intercept = mean_y;
    }

    // ---------------------------
    // Compute R^2
    // ---------------------------
    double ss_tot = 0.0, ss_res = 0.0;
    for (size_t i = 0; i < x_vals.size(); ++i) {
        double pred = intercept + slope * x_vals[i];
        ss_res += (y_vals[i] - pred) * (y_vals[i] - pred);
        ss_tot += (y_vals[i] - mean_y) * (y_vals[i] - mean_y);
    }

    double r2 = (ss_tot == 0.0) ? 0.0 : (1.0 - (ss_res / ss_tot));

    // Store the model in the class (only once)
    modelSlope = slope;
    modelIntercept = intercept;
    modelR2 = r2;

    // ---------------------------
    // Print the LM summary exactly once
    // ---------------------------
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "\nLinear regression model generated from " 
                  << samples.size() << " samples:\n";
    std::cout << "Independent variable: commute_time_minutes\n";
    std::cout << "Dependent variable: travel_distance_miles\n";
    std::cout << "Slope (β1):      " << slope << "\n";
    std::cout << "Intercept (β0):  " << intercept << "\n";
    std::cout << "R^2:             " << r2 << "\n";

    // -------------------------------------------
    // Print sample data used for model generation
    // By default, we only print the first 20 samples
    //
    // NOTE: To print ALL samples instead of the first 20,
    //       simply change:
    //            int maxToPrint = 20;
    //       to:
    //            int maxToPrint = samples.size();
    // -------------------------------------------
    std::cout << "\nSample data used for LM generation:\n";
    std::cout << "index,time_min,distance_miles,home_id,work_id\n";

    int maxToPrint = 20;   // <-- CHANGE TO samples.size() TO PRINT ALL
    maxToPrint = std::min(maxToPrint, static_cast<int>(samples.size()));

    for (int i = 0; i < maxToPrint; i++) {
        const auto& s = samples[i];
        std::cout << i << ","
                  << s.time << ","
                  << s.distance << ","
                  << s.homeID << ","
                  << s.workID << "\n";
    }

    // done
}

RadiusFilterWorkBuildingAssigner::RadiusFilterWorkBuildingAssigner
            (const OSMData& model, const int jwtrnsIdx,
            const int jwmnpIdx, const int offSqFtPer, 
            const int avgSpeed, int lmNumSamples) :
    model(model), jwtrnsIdx(jwtrnsIdx), jwmnpIdx(jwmnpIdx),
    offSqFtPer(offSqFtPer), avgSpeed(avgSpeed), lmNumSamples(lmNumSamples),
    nextBldIndex(0), modelSlope(0.0), modelIntercept(0.0), modelR2(0.0) {
    std::string slurmID = getenv("SLURM_JOB_ID") != nullptr ? getenv("SLURM_JOB_ID") : "";
    std::string mpiRank = std::to_string(MPI_GET_RANK());
    std::string statsFileName = "stats_job" + slurmID + "_rank" + mpiRank + ".txt";
    stats.open(statsFileName);
}

std::tuple<int, int>
RadiusFilterWorkBuildingAssigner::getBldRange(const int bldCount) const {
    const int bldPerRank = bldCount / MPI_GET_SIZE();
    const int startIdx   = bldPerRank * MPI_GET_RANK();
    const int endIdx     = (MPI_GET_RANK() == MPI_GET_SIZE() - 1 ?
                            bldCount : startIdx + bldPerRank);
    return {startIdx, endIdx};
}

long
RadiusFilterWorkBuildingAssigner::getNextBldIndex() {
    long nextIndex = -1;
#pragma omp critical (bldCS)
    {  // start critical section
#ifdef HAVE_LIBMPI
        const long increment_value = 1;
        // Lock the window on the root process
        MPI_Win_lock(MPI_LOCK_SHARED, RANK_0, 0, bldIdxWin);
        MPI_Fetch_and_op(&increment_value, &nextIndex, MPI_LONG, RANK_0, 0,
                         MPI_SUM, bldIdxWin);
        MPI_Win_unlock(RANK_0, bldIdxWin);  // Unlock the window
#else
        nextIndex = nextBldIndex;
        nextBldIndex++;
#endif
    }  // end critical section
    // std::cout << "Returning nextIndex = " << nextIndex << std::endl;
    return nextIndex;
}

void
RadiusFilterWorkBuildingAssigner::
processBuilding(const long idx, const long bldId, Building& bld,
                BuildingMap& nonHomeBuildings) {
    UNUSED_PARAM(idx);
    // std::cout << "Processing building #" << bldId
    //           << "(index: " << idx << ") Rank #" 
    //           << MPI_GET_RANK() << ", thread #" << omp_get_thread_num()
    //           << std::endl;

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

            // ---------------------------
            // Use linear-model predicted dist +/- window to limit candidates
            // ---------------------------
            // Compute predicted dist from the model,
            // (distance = intercept + slope * time)
            double predictedDistance = modelIntercept + modelSlope 
                                       * static_cast<double>(travelTime);

            // Define window - using the 0.7 / 1.3 factors discussed earlier
            double minDist = predictedDistance * 0.7;
            double maxDist = predictedDistance * 1.3;

            /* To keep compatibility with existing signature, we keep calling
               getCandidateWorkBuildings but we will provide min/max travel-time
               range values based on distance window by converting the dists to
               approx. times using avgSpeed. However, to keep the 
               filtering precise we will modify getCandidateWorkBuildings 
               to accept these distances directly.
            
               For minimal intrusive changes, we:
               call getCandidateWorkBuildings with current time-based params,
               but immediately filter its output by haversine distance window.
            
               Keeps existing behavior but applies model-based filtering early.
            */
            for (int slack = 0; slack < timeMargin; slack++) {
                BuildingList candidateWorkBlds =
                    getCandidateWorkBuildings(bld, nonHomeBuildings,
                                              travelTime, travelTime, slack);

                // Now filter candidateWorkBlds based on haversine dist window
                BuildingList filteredBlds;
                filteredBlds.reserve(candidateWorkBlds.size());
                for (auto &cb : candidateWorkBlds) {
                    double d = getDistance(bld.wayLat, bld.wayLon, 
                                           cb.wayLat, cb.wayLon);
                    if ((d >= minDist) && (d <= maxDist)) {
                        filteredBlds.push_back(cb);
                    }
                }

                if (!filteredBlds.empty()) {
                    const long wrkBldId =
                        assignWorkBuilding(model, bld, nonHomeBuildings, 
                                           filteredBlds, ppl, timeMargin);
                    if (wrkBldId != -1) {
                        ppl.setWorkBuilding(bldId, wrkBldId);
                    }
                }
            }
        }
    }
}

void
RadiusFilterWorkBuildingAssigner::assignWorkBuilding(int argc, char *argv[]) {
    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);
    std::cout << "Generating Schedule..." << std::endl;
#ifdef HAVE_LIBMPI
    // Create the window to handle our global building index counter
    nextBldIndex = 0;
    MPI_Win_create(&nextBldIndex, sizeof(long), sizeof(long), MPI_INFO_NULL,
                   MPI_COMM_WORLD, &bldIdxWin);
#endif

    // ---------------------------
    // Parse command line for --lm-num-samples (default comes from constructor)
    // ---------------------------

    // Classify buildings by their types in order to reduce the number
    // of buildings we iterate on for different operations.
    auto [homeBuildings, nonHomeBuildings, homeBldIdList] =
        getHomeAndNonHomeBuildings(model.buildingMap);

    // ---------------------------
    // Generate linear model once (distance ~ time) using random pairs
    // This prints model summary once and a small sample of the rows used.
    // ---------------------------
    generateLinearModel(lmNumSamples, homeBuildings, 
                        nonHomeBuildings, homeBldIdList);

    // Process buildings using multiple threads
#pragma omp parallel
    {
        // Process building by building using a global counter
        const long MaxBldIdx = homeBldIdList.size();
        for (long idx = getNextBldIndex(); idx < MaxBldIdx;
             idx = getNextBldIndex()) {
            const auto bldId = homeBldIdList.at(idx);
            auto& bld  = homeBuildings.at(bldId);
            if (bld.households.empty()) {
                continue;  // no households in this home
            }
            // Have helper method process this building
            processBuilding(idx, bldId, bld, nonHomeBuildings);

            if (idx % 1000 == 0) {
                const double percent = idx * 100 / homeBldIdList.size();
                std::cout << "Progress: curr building index " << idx
                          << " (" << percent << "% completed)" << std::endl;
            }
        }  // per-thread for-loop

        std::cout << "Thread #" << omp_get_thread_num() << " on MPI Rank #"
                  << MPI_GET_RANK() << " has finished at time "
                  << getCurrentTimestamp() << std::endl;
    }  // OMP parallel section
    std::cout << "MPI process with Rank " << MPI_GET_RANK()
              << " has completed processing at time "
              << getCurrentTimestamp() << std::endl;

#ifdef HAVE_LIBMPI
    // Create the window to handle our global building index counter
    MPI_Win_free(&bldIdxWin);
#endif
}

std::tuple<BuildingMap, BuildingMap, std::vector<size_t>>
RadiusFilterWorkBuildingAssigner::getHomeAndNonHomeBuildings
                                  (const BuildingMap& buildingMap) const {
    BuildingMap homeBuildings, nonHomeBuildings;
    std::vector<size_t> homeBldIdList;
    for (auto item = buildingMap.begin(); item != buildingMap.end(); item++) {
        if (item->second.isHome) {
        if (!item->second.households.empty()) {
                homeBuildings[item->first] = item->second;
                homeBldIdList.push_back(item->first);
        }
        } else {
            Building bld = item->second;
            // Initialize capacity of this office building based on
            // its square footage.
            bld.population = bld.getArea() / offSqFtPer;
            nonHomeBuildings[bld.id] = bld;
        }
    }

    // Since we are filtering out buildings with no households above
    // the following assert no longer holds and hence is commented out
    // ASSERT(homeBuildings.size() + nonHomeBuildings.size() ==
    //        buildingMap.size());

    std::cout << "# of non-home buildings: " << nonHomeBuildings.size() << '\n';
    std::cout << "# of home buildings: " << homeBuildings.size() << std::endl;

    return {homeBuildings, nonHomeBuildings, homeBldIdList};
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

    // std::cout << "# of candidate work locations: " << candidateBlds.size()
    //           << std::endl;
    if (candidateBlds.empty()) {
#pragma omp critical(cout)
        std::cout << "Unable to find candidate buildings for time: "
                  << minTravelTime << " for building: " << srcBld.id
                  << ". Using max distance building: " << maxDistBld.id
                  << ", at a distance of: " << maxDist << std::endl;
        candidateBlds.push_back(maxDistBld);
    }
    return candidateBlds;
}

long
RadiusFilterWorkBuildingAssigner::assignWorkBuilding(const OSMData& model,
                                            const Building& bld,
                                            BuildingMap& nonHomeBuildings,
                                            BuildingList& candidateWorkBlds,
                                            const PUMSPerson& person,
                                            const int timeMargin) {
    Stopwatch timer;
    timer.start();
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
            continue;  // This building is not acceptable
        }

        // Found a matching building
        wrkBld.population--;
        // Also update the actual building entry
        nonHomeBuildings[wrkBld.id].population--;

#pragma omp critical(cout)
        stats << "    Assign Building " << wrkBld.id << "(in ring: "
              << wrkBld.attributes << ") to person " << person.getPerID()
              << " in building " << bld.id << "(in ring: " << bld.attributes
              << ") after checking " << bldCount << " out of "
              << candidateWorkBlds.size() << " buildings. "
              << "Person needs time: " << travelTime
              << " and the assigned building has time: "
              << timeInMinutes << ".  Compute time: "
              << timer.elapsedTime() << " milliseconds" << std::endl;
        return wrkBld.id;
    }

#pragma omp critical(cout)
    {
        stats << "    Unable to find building for person " << person.getPerID()
              << " in building " << bld.id << "(in ring: " << bld.attributes
              << ") after checking " << candidateWorkBlds.size()
              << " buildings. Person needs time: " << travelTime
              << ".  Compute time: "
              << timer.elapsedTime() << " milliseconds" << std::endl;
        person.write(std::cerr);
    }
    return -1;
}

#endif
