#ifndef OSM_WAY_CPP
#define OSM_WAY_CPP

//------------------------------------------------------------
//
// This file is part of HAPLOS <http://pc2lab.cec.miamiOH.edu/>
//
// Human  Population  and   Location  Simulator (HAPLOS)  is
// free software: you can  redistribute it and/or  modify it
// under the terms of the GNU  General Public License  (GPL)
// as published  by  the   Free  Software Foundation, either
// version 3 (GPL v3), or  (at your option) a later version.
//
// HAPLOS is distributed in the hope that it will  be useful,
// but   WITHOUT  ANY  WARRANTY;  without  even  the IMPLIED
// WARRANTY of  MERCHANTABILITY  or FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Miami University and the HAPLOS  development team make no
// representations  or  warranties  about the suitability of
// the software,  either  express  or implied, including but
// not limited to the implied warranties of merchantability,
// fitness  for a  particular  purpose, or non-infringement.
// Miami  University and  its affiliates shall not be liable
// for any damages  suffered by the  licensee as a result of
// using, modifying,  or distributing  this software  or its
// derivatives.
//
// By using or  copying  this  Software,  Licensee  agree to
// abide  by the intellectual  property laws,  and all other
// applicable  laws of  the U.S.,  and the terms of the  GNU
// General  Public  License  (version 3).  You  should  have
// received a  copy of the  GNU General Public License along
// with HAPLOS.  If not, you may  download copies  of GPL V3
// from <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------

#include "OsmWay.h"

OsmWay::OsmWay(const long id) : id(id) {
    // Nothing else to be done.
}

OsmWay::~OsmWay() {
    // Nothing else to be done.
}

void
OsmWay::addNode(const long id) {
    nodeList.push_back(id);
}

void
OsmWay::print(const NodeMap& nodes, std::ostream& os) const {
    os << "way[id=" << id << "], ";
    os << dynamic_cast<const OsmTags&>(*this);
    os << ", nodes={\n";
    for (const long id : nodeList) {
        NodeMap::const_iterator entry = nodes.find(id);
        if (entry != nodes.end()) {
            // Found node information print details.
            os << "  " << entry->second << std::endl;
        } else {
            // Node not found!
            os << "  node[id=" << id << ", lat=?, lon=?], tags={}";
        }
    }
    os << "}\n";
}

std::ostream& operator<<(std::ostream& os, const OsmWay& way) {
    os << "way[id=" << way.id << "], nodes={";
    std::string delim = "";
    for (const auto& id : way.nodeList) {
        os << delim << id;
        delim = ", ";
    }
    os << "}, ";
    os << dynamic_cast<const OsmTags&>(way);
    return os;
}

#endif
