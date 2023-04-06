#ifndef PUMS_HOUSEHOLD_CPP
#define PUMS_HOUSEHOLD_CPP

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

#include "PUMSHousehold.h"
#include "Utilities.h"

// The static bulding recoding values to provide a better estimate for
// building sizes from the getSize method. For example, This is needed
// because detached single-family homes that have a code of 2 should
// have higher size than attached-single-family homes.  
const std::vector<int> PUMSHousehold::BldRecode =
    {0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 1};

PUMSHousehold::PUMSHousehold(const std::string houseInfo, int bedRooms,
                             int bldCode, int pumaID, int wgtp, int hincp) :
    houseInfo(houseInfo), bedRooms(bedRooms), bld(bldCode), pumaID(pumaID),
    wgtp(wgtp), hincp(hincp) {
    ASSERT(bldCode >= 0);
    ASSERT(bldCode <= 10);
    // Nothing else to be done.
}

int
PUMSHousehold::getAptInBld() const {
    // Convert the building code to number of rooms to ease sorting in
    // future.
    static const std::vector<int> Code2Apts = {99, 1, 1, 1, 2,
                                               4, 7, 15, 35, 50, 0};
    ASSERT(bld >= 0);
    ASSERT(bld < (int) Code2Apts.size());
    return Code2Apts[bld];
}

void
PUMSHousehold::addPersonInfo(const int occurrence, const std::string& info) {
    pepWtInfo.push_back(PepWtInfo(occurrence, info));
}

std::string
PUMSHousehold::getInfo(const int i, int& pepCount) const {
    // Ensure i is valid.
    ASSERT((i >= 0) && (i < getCount()));
    std::string info;  // household information to be returned.
    pepCount = 0;

    // Iterate over each person in this household and include them in
    // the family information.
    for (size_t per = 0; (per < pepWtInfo.size()); per++) {
        const double perScale = pepWtInfo[per].first * 1.0 / wgtp;
        // Compute number of occurrences of a person. Note that the
        // occurrence can be zero-or-more depending on the
        // characteristics of this household.
        int occurs = (perScale * (i + 1)) - (perScale * i);
        // If we have only 1 person entry for this household, then
        // include that one person information.  In some cases, in
        // PUMS data, the wgtp value is slightly higher than number of
        // people.
        if (pepWtInfo.size() == 1) {
            occurs = std::max(1, occurs);
        }
        // Add the number of occurrences to the household information.
        while (occurs-- > 0) {
            info += pepWtInfo[per].second + ";";
            pepCount++;
        }
    }

    // Note that in some fractional cases, household info can be an
    // empty string.  Calling pop_back on empty string causes weird error
    if (!info.empty()) {
        info.pop_back();  // Remove trailing semicolon.
        info = houseInfo + ' ' + std::to_string(bedRooms)
            + ' ' + std::to_string(bld) + ' ' + std::to_string(pumaID)
            + ' ' + std::to_string(wgtp) + ' ' + std::to_string(hincp)
            + ' ' + info;
    }
    return info;
}

#endif
