#ifndef WAY_CPP
#define WAY_CPP

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

#include "Way.h"
#include "Utilities.h"

void
Way::write(std::ostream& os, const bool writeHeader,
           const std::string& delim) const {
    // Write optional header if requested
    if (writeHeader) {
        os << "# Way"        << delim << "ID"        << delim
           << "kind"         << delim
           << "maxSpeed"     << delim << "isDeadEnd" << delim
           << "isOneWay"     << delim << "hasLoops"  << delim
           << "numBuildings" << delim
           << "nodeID"       << delim << "[nodeID...]\n";
    }
    // Write the information for this way
    os << "way"     << delim << id        << delim << kind     << delim
       << maxSpeed  << delim << isDeadEnd << delim << isOneWay << delim
       << hasLoop   << delim << numBuildings;
    // Write the nodeID's for this way
    for (const long nodeID : nodeList) {
        os << delim << nodeID;
    }
    os << std::endl;
}

void
Way::read(std::istream& is) {
    // Read the information for this way
    std::string way;  // Dummy word to be discarted
    int kindId = 0;
    is >> way >> id >> kindId >> maxSpeed >> isDeadEnd >> isOneWay
       >> hasLoop   >> numBuildings;
    // Convert kindId from integer to enumeration
    kind = Way::Kind(kindId);
    // We assume that rest of the input stream has the ID of the nodes
    // for this way.
    nodeList.clear();
    long nodeID;
    while (is >> nodeID) {
        nodeList.push_back(nodeID);
    }
    ASSERT(!nodeList.empty());
}

#endif
