#ifndef OSM_BUILDING_CPP
#define OSM_BUILDING_CPP

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

#include "OsmBuilding.h"
#include <iomanip>

OsmBuilding::OsmBuilding(const OsmBuilding::Kind kind, const long id,
                         const double latitude, const double longitude,
                         const double radius) :
    kind(kind), id(id), latitude(latitude), longitude(longitude),
    radius(radius) {
    // Nothing else to be done here.
}

OsmBuilding::~OsmBuilding() {
    // Nothing else to be done here.
}

double
OsmBuilding::operator-(const OsmBuilding& other) const {
    return Utils::getDistance(this->latitude, this->longitude,
                              other.latitude, other.longitude);    
}

std::ostream& operator<<(std::ostream& os, const OsmBuilding& bld) {
    os << "bld[id=" << bld.id << ", kind=" << bld.kind
       << ", lat=" << std::setprecision(9)
       << bld.latitude  << ", lon=";
    os << bld.longitude << ", rad=" << bld.radius << "]";
    return os;
}

#endif
