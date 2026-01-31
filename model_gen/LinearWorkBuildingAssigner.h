#ifndef LINEAR_WORK_BUILDING_ASSIGNER_H
#define LINEAR_WORK_BUILDING_ASSIGNER_H

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

#include "OSMData.h"

/** An alias to refer to a vector of buildings **/
using BuildingList = std::vector<Building>;

/** An alias for a map of buildings with the building ID as the key */
using BuildingMap = std::unordered_map<long, Building>;

class LinearWorkBuildingAssigner {
public:
    /**
       The only constructor for this class.
     */
    LinearWorkBuildingAssigner(const OSMData& model,
                               const int jwtrnsIdx,
                               const int jwmnpIdx,
                               const int offSqFtPer,
                               const int avgSpeed,
                               int lmNumSamples);

    /** Dummy destructor (per coding conventions). */
    ~LinearWorkBuildingAssigner() {}

    /** Entry point for work-building assignment */
    void assignWorkBuilding(int argc, char *argv[]);

protected:
    /**
       Helper method to get the range of buildings this MPI process
       should operate on.
     */
    std::tuple<int, int> getBldRange(const int bldCount) const;

    /**
       Split buildings into home and non-home sets.
     */
    std::tuple<BuildingMap, BuildingMap, std::vector<size_t>>
    getHomeAndNonHomeBuildings(const BuildingMap& buildingMap) const;

    /**
       Generate candidate non-home buildings using time approximation.
     */
    BuildingList getCandidateWorkBuildings(const Building& bld,
                                           const BuildingMap& nonHomeBlds,
                                           const int minTravelTime,
                                           const int maxTravelTime,
                                           const int timeMargin = 1) const;

    /**
       Assign the FIRST valid work building from the candidate list.
     */
    long assignWorkBuilding(const OSMData& model,
                            const Building& bld,
                            BuildingMap& nonHomeBuildings,
                            BuildingList& candidateWorkBlds,
                            const PUMSPerson& person,
                            const int timeMargin);

    /**
       Process all households and people in a single home building.
     */
    void processBuilding(const long idx,
                         const long bldId,
                         Building& bld,
                         BuildingMap& nonHomeBuildings);

    /**
       Thread-safe method to get the next building index.
     */
    long getNextBldIndex();

private:
    /** Reference to the OSM model */
    const OSMData& model;

    /** jwtrns column index */
    const int jwtrnsIdx;

    /** jwmnp column index */
    const int jwmnpIdx;

    /** Average square feet per office worker */
    const int offSqFtPer;

    /** Average travel speed (mph) */
    const int avgSpeed;

    /** Number of samples used to generate the linear model */
    int lmNumSamples;

    /** Global building index counter */
    long nextBldIndex = 0L;

#ifdef HAVE_LIBMPI
    /** MPI window for global building index */
    static MPI_Win bldIdxWin;
#endif

    /** Per-process statistics output */
    std::ofstream stats;

    // ---------------------------
    // Linear model parameters
    // distance ≈ intercept + slope * time
    // ---------------------------
    double modelSlope;
    double modelIntercept;
    double modelR2;

    /**
       Generate the linear distance–time model once.
     */
    void generateLinearModel(int lmNumSamples,
                             const BuildingMap& homeBuildings,
                             const BuildingMap& nonHomeBuildings,
                             const std::vector<size_t>& homeBldIdList);
};

#endif
