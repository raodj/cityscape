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

PUMSHousehold::PUMSHousehold(const std::string houseInfo, int bedRooms,
                             int bldCode, int pumaID, int pwgtp) :
    houseInfo(houseInfo), bedRooms(bedRooms), bld(bldCode), pumaID(pumaID),
    remaining(pwgtp) {
    ASSERT(bldCode >= 0);
    ASSERT(bldCode <= 10);
    // Nothing else to be done.
}

int
PUMSHousehold::getAptInBld() const {
    // Convert the building code to number of rooms to ease soring in
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

#endif
