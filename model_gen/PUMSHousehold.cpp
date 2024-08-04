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

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "PUMSHousehold.h"
#include "Utilities.h"

// The static bulding recoding values to provide a better estimate for
// building sizes from the getSize method. For example, This is needed
// because detached single-family homes that have a code of 2 should
// have higher size than attached-single-family homes.  
const std::vector<int> PUMSHousehold::BldRecode =
    {0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 1};

// The list of column names specified as command-line args.  This is
// set in ModelGenerator::processArgs method.
std::vector<std::string> PUMSHousehold::pumsHouColNames;

// Unique person ID value
long PUMSHousehold::perIDcounter = 0;

PUMSHousehold::PUMSHousehold(const std::string houseInfo, int bedRooms,
                             int bldCode, int pumaID, int wgtp, int hincp,
                             int people) :
    houseInfo(houseInfo), bedRooms(bedRooms), bld(bldCode), pumaID(pumaID),
    wgtp(wgtp), hincp(hincp), people(people) {
    ASSERT(bldCode >= 0);
    ASSERT(bldCode <= 10);
    // Nothing else to be done.
}

PUMSHousehold::PUMSHousehold(const PUMSHousehold& hld, int bldId, int people,
                             const std::vector<PUMSPerson>& peopleInfo) :
    houseInfo(hld.houseInfo), bedRooms(hld.bedRooms), bld(hld.bld),
    pumaID(hld.pumaID), wgtp(hld.wgtp), hincp(hld.hincp), people(people),
    buildingID(bldId), peopleInfo(peopleInfo) {
    ASSERT(bld >= 0);
    ASSERT(bld <= 10);
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
PUMSHousehold::addPersonInfo(const int occurrence, const PUMSPerson& person) {
    pepWtInfo.push_back(PepWtInfo(occurrence, person));
}

std::vector<PUMSPerson>
PUMSHousehold::getInfo(const int i, int& pepCount) const {
    // Ensure i is valid.
    ASSERT((i >= 0) && (i <= getCount()));
    std::vector<PUMSPerson> info;  // persons information to be returned.
    pepCount = 0;

    // Iterate over each person in this household and include them in
    // the family information. 
    for (size_t per = 0; (per < pepWtInfo.size()); per++) {
        /** April 9 2023 -- raodm -- The following formula is
            incorrect in the context of households. Replacing with a
            simpler/correct approach to generate the same number of
            people as in the household */
        // const double perScale = pepWtInfo[per].first * 1.0 / wgtp;
        const double perScale = 1.0;
        
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
            // info += pepWtInfo[per].second.getInfo() + ";";
            PUMSPerson person = pepWtInfo[per].second;
            person.setPerID(perIDcounter++);
            info.push_back(person);
            pepCount++;
        }
    }
    return info;
}

void
PUMSHousehold::write(std::ostream& os, const bool writeHeader,
                     const std::string& delim) const {
    if (writeHeader) {
        os << "# Hld bldID";
        bool first = true;
        for (const std::string& col : pumsHouColNames) {
            os << (first ? ' ' : ',') << col;
            first = false;
        }
        os << " bedRooms BLDtype pumaID WGTP HINCP #people peopleIDs...\n";
    }

    // Write the actual data for this household
    os << "hld"  << delim    << buildingID << delim << std::quoted(houseInfo)
       << delim  << bedRooms << delim      << bld   << delim << pumaID
       << delim  << wgtp     << delim      << hincp << delim << people;

    // Write the IDs of each person record 
    for (const auto person : peopleInfo) {
        os << delim << person.getPerID();
    }
    os << '\n';
}

void
PUMSHousehold::read(std::istream& is, const PersonMap& personMap) {
    // Read information for this household
    std::string hld;     // string to be read and discarted
    is >> hld;
    is >> buildingID  >> std::quoted(houseInfo) >> bedRooms >> bld
       >> pumaID >> wgtp >> hincp >> people;
    // Read each person's ID and look-up the person's info in the
    // person map to get the information about the person.
    peopleInfo.resize(people);
    for (int i = 0; (i < people); i++) {
        long perID;
        is >> perID;
        const auto entry = personMap.find(perID);
        if (entry == personMap.end()) {
            throw std::runtime_error("Did not find entry for perID " +
                                     std::to_string(perID) + 
                                     " for household " + houseInfo);
        }
        // Add this person entry to the household.
        peopleInfo[i] = entry->
            second;
    }
}

#endif
