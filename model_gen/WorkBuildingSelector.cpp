#ifndef WORK_BUILDING_SELECTOR_CPP
#define WORK_BUILDING_SELECTOR_CPP

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
#include <random>
#include <numeric>
#include <iostream>
#include <chrono>
#include "WorkBuildingSelector.h"
#include "Utilities.h"
#include "PathFinder.h"

// Just a shortcut to refer to TimeEstimates
using TimeEstimate = WorkBuildingSelector::TimeEstimate;

// Convenience use to measure time
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

void
WorkBuildingSelector::genOrUseTrvlEst(const int numBldPairs,
                                      const std::string& useTrvlEstFile,
                                      const std::string& outTrvlEstFile) {
    if (useTrvlEstFile.empty()) {
        // An travel time estimate matrix input file is not
        // specified. So we need to generate it.
        generateTravelTimeEstimates(numBldPairs);
        if (!outTrvlEstFile.empty()) {
            std::ofstream outEst(outTrvlEstFile);
            if (!outEst.good()) {
                std::string err = "Error writing to " + outTrvlEstFile;
                throw std::runtime_error(err);
            }
            printTimeEstimatesMatrix(outEst);
        }
    } else {
        // Load the schedule from a given input text file
        std::ifstream inEst(useTrvlEstFile);
        if (!inEst.good()) {
            std::string err = "Error reading travel estimates from " +
                useTrvlEstFile;
            throw std::runtime_error(err);
        }
        std::cout << "Reading travel estimates from " << useTrvlEstFile << '\n';
        loadTravelTimeEstimates(inEst);
        std::cout << "Loaded travel estimates from " << useTrvlEstFile << '\n';
    }
}

void
WorkBuildingSelector::generateTravelTimeEstimates(const int numPairs) {
    // First build the list of work-buildings in each population ring.
    buildPopRingBuildingList();
    // Pre-allocate space to store nxn entries.
    const int numRings = model.popRings.size();
    timeEstimates.resize(numRings, std::vector<TimeEstimate>(numRings));
    // Build time estimates between pairs of rings in the model
    for (int rngIdx1 = 0; rngIdx1 < numRings; rngIdx1++) {
        const auto startTime = high_resolution_clock::now();
        if (rngIdx1 % 1 == 0) {
#pragma omp critical
            std::cout << "Working on rngIdx: " << rngIdx1
                      << " on thread " << omp_get_thread_num() << std::flush;
        }
#pragma omp parallel for schedule(guided)        
        for (int rngIdx2 = 0; rngIdx2 < numRings; rngIdx2++) {
            if (rngIdx1 <= rngIdx2) {
                DEBUG(std::cout << "Working on ring pair: " << rngIdx1 << ", "
                      << rngIdx2 << std::flush);
                const TimeEstimate est =
                    estimateTime(rngIdx1, rngIdx2, numPairs);
                
                timeEstimates.at(rngIdx1).at(rngIdx2) = est;
                timeEstimates.at(rngIdx2).at(rngIdx1) = est;

                DEBUG(std::cout << ", estimate: " << est.mean << "+-"
                      << est.stdev << std::endl);
            }
        }
        const auto endTime = high_resolution_clock::now();
        const auto duration = duration_cast<milliseconds>(endTime - startTime);
        std::cout << ", elapsedTime = " << duration.count() << " msecs\n";
    }
}


void
WorkBuildingSelector::buildPopRingBuildingList() {
    // Create initial space for each population ring.
    popRingWrkBld.resize(model.popRings.size());
    // Now store building IDs for each population ring using the
    // attribute value associated with each building.
    for (const auto& [bldId, bld] : model.buildingMap) {
        const size_t rngIdx = bld.attributes;
        ASSERT ((rngIdx >= 0) && (rngIdx < popRingWrkBld.size()));
        if (!bld.isHome) {
            popRingWrkBld[rngIdx].push_back(bldId);
        }
    }
}

WorkBuildingSelector::TimeEstimate
WorkBuildingSelector::estimateTime(const size_t ringIdx1, const size_t ringIdx2,
                                   const int numPairs) const {
    // Basic sanity checks to ensure consistent start point.
    ASSERT( ringIdx1 < popRingWrkBld.size() );
    ASSERT( ringIdx2 < popRingWrkBld.size() );
    ASSERT( numPairs > 0 );
    if (popRingWrkBld[ringIdx1].empty() || popRingWrkBld[ringIdx2].empty()) {
        // one of the rings is empty. So there is nothing to do.
        DEBUG(std::cerr << "No work buildings in ring "
              << ringIdx1 << "(" << popRingWrkBld[ringIdx1].size()
              << ") or in ring " 
              << ringIdx2 << "(" << popRingWrkBld[ringIdx2].size()
              << ")\n");
        return TimeEstimate{-1, -1};
    }
    // Random number generator to select a pair of buildings from
    // reach specified ring.    
    std::default_random_engine rndGen;
    std::uniform_int_distribution<> rng1Bld(0, popRingWrkBld[ringIdx1].size() - 1);
    std::uniform_int_distribution<> rng2Bld(0, popRingWrkBld[ringIdx2].size() -  1);
    // Generate the specified number of random pairs of buildings and
    // record travel times.
    std::vector<double> travelTimes;
    for (int pairCnt = 0; pairCnt < numPairs; pairCnt++) {
        const int bldIdx1 = rng1Bld(rndGen), bldIdx2 = rng2Bld(rndGen);
        const auto trvlTime = getTime(popRingWrkBld.at(ringIdx1).at(bldIdx1),
                                      popRingWrkBld.at(ringIdx2).at(bldIdx2));
        if (trvlTime != -1) {
            travelTimes.push_back(trvlTime);
        }
    }
    if (travelTimes.empty()) {
        return {-1, -1};   // No estimates obtained.
    }
    
    // Compute mean of the values.  The approach below is susceptible
    // to overflow or underflow for huge or tiny values.
    const double sum  = std::accumulate(travelTimes.begin(),
                                        travelTimes.end(), 0.0);
    const double mean = sum / travelTimes.size();
    // Compute standard deviation of values.
    const double sq_sum = std::inner_product(travelTimes.begin(),
                                             travelTimes.end(),
                                             travelTimes.begin(), 0.0);
    const double stdev = std::sqrt(sq_sum / travelTimes.size() - mean * mean);
    return TimeEstimate{mean, stdev};
}

double
WorkBuildingSelector::getTime(const long bldId1, const long bldId2) const {
    PathFinder pf(model);
    const Path path = pf.findBestPath(bldId1, bldId2, true, 0.25, 0.1);
    if (path.size() == 0) {
        return -1;  // no path found!
    }
    double timeInMins = path.back().distance * 60;
    return timeInMins;
}

void
WorkBuildingSelector::printTimeEstimatesMatrix(std::ostream& os) const {
    const int numRings = model.popRings.size();
    for (int rngIdx1 = 0; rngIdx1 < numRings; rngIdx1++) {
        for (int rngIdx2 = 0; rngIdx2 < numRings; rngIdx2++) {
            const auto& est = timeEstimates.at(rngIdx1).at(rngIdx2);
            if (rngIdx2 > 0) {
                os << '\t';
            }
            os << est.mean << ' ' << est.stdev;
        }
        os << '\n';
    }
}

void
WorkBuildingSelector::loadTravelTimeEstimates(std::istream& is) {
    const int numRings = model.popRings.size();
    timeEstimates.resize(numRings, std::vector<TimeEstimate>(numRings));
    for (int rng1 = 0; (rng1 < numRings); rng1++) {
        for (int rng2 = 0; (rng2 < numRings); rng2++) {
            if (!is.good()) {
                std::string msg = "Error reading travel estimate at row " +
                    std::to_string(rng1) + " and col " + std::to_string(rng2);
                throw std::runtime_error(msg);
            }
            TimeEstimate& te = timeEstimates[rng1][rng2];
            is >> te.mean >> te.stdev;
        }
    }
}


#endif

