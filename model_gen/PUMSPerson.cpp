#ifndef PUMS_PERSON_CPP
#define PUMS_PERSON_CPP

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

#include <sstream>
#include <stdexcept>
#include <iomanip>
#include "PUMSPerson.h"
#include "Utilities.h"
#include "ScheduleEntry.h"

// The static instance variable to hold column names
std::vector<std::string> PUMSPerson::colTitles;

PUMSPerson::PUMSPerson(const std::string& hldID,
                       const std::vector<int>& colIndexs,
                       const std::vector<std::string>& csvInfo) {
    householdID = hldID;
    info.resize(colIndexs.size());
    for (size_t i = 0; (i < colIndexs.size()); i++) {
        const int colNum = colIndexs[i];
        if (csvInfo[colNum].empty()) {
            // If information is not available then we assume -1.
            info[i] = -1;
        } else {
            info[i] = std::stol(csvInfo[colNum]);
        }
    }
}

std::string
PUMSPerson::getInfo() const {
    std::ostringstream ss;
    for (const auto val : info) {
        ss << val << ',';
    }
    std::string str = ss.str();
    if (!str.empty()) {
        str.pop_back();
    }
    return str;
}

void
PUMSPerson::write(std::ostream& os, const bool writeHeader,
                  const std::string& delim) const {
    if (writeHeader) {
        os << "# Per perID HldID #info";
        for (const std::string& col : colTitles) {
            os << ' ' << col;
        }
        os << " schedule\n";
    }

    // Write the actual data for this person
    os << "per" << delim << perID << delim << householdID
       << delim << info.size();
    for (const auto& i : info) {
        os << delim << i;
    }
    os << delim << std::quoted(schedule) << '\n';
}

void
PUMSPerson::read(std::istream& is) {
    // Read information for this person
    std::string per;     // fixed string to be read and discarted
    is >> per;
    int numInfo;   // Number of info entries to be read.
    is >> perID  >> householdID >> numInfo;
    info.resize(numInfo);
    for (int i = 0; i < numInfo; i++) {
        is >> info[i];
    }
    is >> std::quoted(schedule);
}

void
PUMSPerson::readColTitles(const std::string& titleCommentLine) {
    std::istringstream is(titleCommentLine);
    std::string ignore, per, hldID, title;
    int numInfo = 0;
    //     '#'       'per'     perID       hldID
    is >> ignore >> per >> ignore >> hldID >> numInfo;
    ASSERT( per == "per" );
    ASSERT( hldID == "householdID" );
    // Load the column information into the colTitles
    colTitles.resize(numInfo);
    for (int i = 0; (i < numInfo); i++) {
        is >> title;
        colTitles[i] = title;
    }
    // Read the last schedule entry.
    is >> ignore;
    ASSERT(ignore == "schedule");
}

void
PUMSPerson::setColumnTitles(const std::vector<std::string>& colNames,
                            const std::vector<int>& colIndexs) {
    colTitles.resize(colIndexs.size());
    for (size_t i = 0; (i < colIndexs.size()); i++) {
        colTitles[i] = colNames[colIndexs[i]];
    }
}

int
PUMSPerson::getIntegerInfo(const int infoIdx) const {
    const auto infoArray = split(this->getInfo(), ",");
    int ret;

    if ((size_t)infoIdx >= infoArray.size()) {
        throw std::out_of_range("infoIdx is too large");
    }

    try {
        ret = std::stoi(infoArray.at(infoIdx));
    } catch (const std::invalid_argument& e) {
        throw;
    }

    return ret;

}

void
PUMSPerson::setWorkBuilding(const long homeBldId, const long bldId,
                            const long leave2workTime,
                            const long leaveFromWorkTime) {
    const ScheduleEntry toWork(1, 5, leave2workTime, bldId);
    const ScheduleEntry toHome(1, 5, leaveFromWorkTime, leaveFromWorkTime);
    schedule += toWork.to_string();
    schedule += toHome.to_string();
}

#endif
