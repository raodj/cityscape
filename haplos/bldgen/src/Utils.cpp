#ifndef UTILS_CPP
#define UTILS_CPP

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

#include <cmath>
#include "Utils.h"

double
Utils::getDistance(double lat1, double lon1, double lat2, double lon2) {
    constexpr double R  = 3958.75587;  // radius of earth in miles
    // Compute distance based on Haversine's formula.
    // See: https://en.wikipedia.org/wiki/Haversine_formula

    // Convenience function to compute have function
    auto hav = [](double coord1, double coord2) {
        const double sinDelta = sin(toRadians(coord1 - coord2) / 2.0);
        return (sinDelta * sinDelta);
    };
    double hav_dist = hav(lat1, lat2) +
        (cos(toRadians(lat1)) * cos(toRadians(lat2)) * hav(lon1, lon2));
    // Now compute actual distance based on radius of earth
    return 2 * R * asin(sqrt(hav_dist));
}

Location
Utils::change(const Location& curr, double latMiles, double lonMiles) {
    constexpr double MilesPerLat = 69.04677;
    const double lon = curr.second + (lonMiles / (cos(toRadians(curr.first)) *
                                                  MilesPerLat));
    const double lat = curr.first + (latMiles / MilesPerLat);
    // Return the new latitude, longitude pair
    return Location(lat, lon);
}

#endif
