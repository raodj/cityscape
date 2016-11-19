#ifndef UTILS_H
#define UTILS_H

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

#include <utility>

/** The Location is just a convenience shortcut to refer to a
    <latitude, longitude> pair of values.  Note that Location::first
    is latitude and Location::second is longitude.
*/
using Location = std::pair<double, double>;

/** A simple class to encapsulate different helper methods used in the
    BuildingGenerator.
*/
class Utils {
public:
    /** Convert angles (latitude/longitude) from degrees to radians

        This is a convenience method to convert angle in degrees to
        radians.  This is required when using trigonometric functions
        as they work in radians and not in degrees.

        \param[in] degrees The angle in degrees to be converted to
        radians.

        \return The specified angle in radians.
    */
    static inline double toRadians(const double degrees) {
        constexpr double PI = 3.1415926;
        return (degrees * PI / 180.0);
    }

    /** Convert angles (latitude/longitude) from radians to degrees

        This is a convenience method to convert angle in radians to
        degrees.  This is required when converting between
        image-pixesl to latitude/longitude.

        \param[in] radians The angle in radians to be converted to
        degrees.

        \return The specified angle in degrees.
    */
    static inline double toDegrees(const double radians) {
        constexpr double PI = 3.1415926;
        return (radians / PI * 180.0);
    }

    /** Returns the great circle distance between 2 points, given
        their latitudes and longitudes.

        The distance between the two points is computed using
        Haversine Formula: https://en.wikipedia.org/wiki/Haversine_formula

        \param[in] lat1 The 1st point's latitude  ( -90 < lat1 < 90)
        
        \param[in] lon1 The 1st point's longitude (-180 < lon1 < 180)
        
        \param[in] lat2 The 2nd point's latitude  ( -90 < lat2 < 90)
        
        \param[in] lon2 The 2nd point's longitude (-180 < lon2 < 180)
        
        \return The distance between the 2 points.
    */
    static double getDistance(double lat1, double lon1,
                              double lat2, double lon2);

    /** Convenience method to determine new latitude and longitude if
        a point is moved by given distance.

        This method computes the resulting latitude and longitude
        value if a point is changed by a given amount of miles of
        latitude and longitude.

        \param[in] curr The current location (latitude, longitude
        pair).

        \param[in] latMiles The change in latitude in miles.  The
        latitude change happens in a constant manner at 69.04677 miles
        per latitude lange.

        \param[in] lonMiles THe change in longitude.  The change in
        longitude is determined based on the latitude value.  Near the
        poles longitude rapidly changes due to spherical nature of
        Earth.

        \return The resulting latitude longitude values.
    */
    static Location change(const Location& curr, double latMiles,
                           double lonMiles);
    
protected:
    // Currently this class does not have any protected methods.

private:
    /** The constructor.

        The constructor is intentionally private so that this class is
        never instantiated.  Instead directly use the static methods
        in this class.
    */
    Utils() {}
};


#endif
