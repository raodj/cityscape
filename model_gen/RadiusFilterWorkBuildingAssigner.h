#ifndef RADIUS_FILTER_WORK_BUILDING_ASSIGNER_H
#define RADIUS_FILTER_WORK_BUILDING_ASSIGNER_H

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

class RadiusFilterWorkBuildingAssigner {
public:
    /**
       The only constructor for this class.  
    */
    RadiusFilterWorkBuildingAssigner(const OSMData& model,
                                     const int jwtrnsIdx,
                                     const int jwmnpIdx,
                                     const int offSqFtPer,
                                     const int avgSpeed) :
        model(model), jwtrnsIdx(jwtrnsIdx), jwmnpIdx(jwmnpIdx),
        offSqFtPer(offSqFtPer), avgSpeed(avgSpeed) {}

    /** A destructor.

        This is a dummy destructor and is present merely to adhere to
        coding conventions.
    */
    ~RadiusFilterWorkBuildingAssigner() {}

    void assignWorkBuilding(int argc, char *argv[]);


protected:
    /**
       This is an internal helper method that is used to get the range
       of buildings that this MPI-process should operate on.

       \param[in] bldCount The number of buildings to be processed by
       all the different ranks.

       \return A tuple with the starting and ending index of the
       buildings to be processed by the caller.
    */
    std::tuple<int, int> getBldRange(const int bldCount) const;
    
    /**
       An internal convenience method to make a list of home and
       non-home buildings so that we iterate on a smaller subset of
       lists (and avoid checks) to make things a tad-bit faster.

       \param[in] buildingMap The map of the buildings from the model.

       \return A pair of building lists with the home and non-home
       buildings (in that order).
     */
    std::tuple<BuildingMap, BuildingMap, std::vector<size_t>>
    getHomeAndNonHomeBuildings(const BuildingMap& buildingMap) const;

    /**
       Internal helper method to return a sorted list of buildings
       that are potential candidates to the given building.

       \param[in] bld The source building from which non-home
       buildings are to be obtained.

       \param[in] nonHomeBlds The list of non-home buildings from
       where the subset of buildings are to be returned.

       \param[in] minTravelTime The minimum anticipated travel time
       (in minutes).

       \param[in] maxTravelTime The maximum anticipated travel time
       (in minutes)

       \param[in] timeMargin Additional fudge factor that is used to
       find buildings around the specified limit (in minutes).
     */
    BuildingList getCandidateWorkBuildings(const Building& bld,
                                           const BuildingMap& nonHomeBlds,
                                           const int minTravelTime,
                                           const int maxTravelTime,
                                           const int timeMargin = 1) const;

    std::unordered_map<long, long>
    assignWorkBuildings(const OSMData& model,
                        const Building& bld,
                        BuildingMap& nonHomeBuildings,
                        BuildingList& candidateWorkBlds,
                        const PUMSPerson& person,
                        const int timeMargin);    

private:
    /**
       Reference to the model to be used for identifying work-buildings.
     */
    const OSMData& model;

    /**
       The index of the jwtrns (type of transport to work) column in
       each person's metadata
    */
    const int jwtrnsIdx;
    
    /**
       The index of the jwmnp (travel time to work) column in each
       person's metadata
    */
    const int jwmnpIdx;

    /**
       The average sq foot per person in an office.
     */
    const int offSqFtPer;

    /**
       The average speed in miles-per-hour to be used to make a rough
       estimate of travel time.
     */
    const int avgSpeed;
};

#endif
