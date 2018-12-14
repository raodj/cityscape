#ifndef UTILITIES_H
#define UTILITIES_H

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

#include <ctime>
#include <cmath>
#include <string>

/** \def ASSERT(x)

    \brief Define a convenient macro for using c asserts.

    Define a custom macro ASSERT (note the all caps) method to be used
    instead of the c \c assert method to enable compiling ASSERT
    usages in and out of the source code.  When the compiler flag \c
    DEVELOPER_ASSERTIONS is specified then the ASSERT call defaults to
    the normal \c assert method.  For example ASSERT (\<<em>boolean
    expression</em>\>)} will be mapped to \c assert (\<<em>boolean
    expression</em>\>).  If the compiler flag \c DEVELOPER_ASSERTIONS
    is not specified then the ASSERT simply gets compiled out.
*/
#ifndef ASSERT
#ifdef DEVELOPER_ASSERTIONS
#include <assert.h>

#define WHEN_ASSERT(x) x
#define ASSERT(x) assert(x)

#else // !DEVELOPER_ASSERTIONS

#define WHEN_ASSERT(x)
#define ASSERT(x)

#endif
#endif

/** \def DEBUG(x)

    \brief Define a convenient macro for conditionally compiling
    additional debugging information.

    Define a custom macro DEBUG (note the all caps) macro to be used
    to conditionally compile in debugging code to generate detailed
    logs.  This helps to minimize code modification to insert and
    remove debugging messages.
*/
#ifndef DEBUG
#ifdef DEBUG_OUTPUT

#define DEBUG(x) x

#else // !DEBUG_OUTPUT

#define DEBUG(x)

#endif
#endif

/**\def END_NAMESPACE(x)

   \brief A convenient macro for ending namespace declarations

   This macro provides a convenient approach for defining namespaces
   in C++ headers and source files.  Using these macros ensures that
   indendation for namespaces do not clutter or eat up screen
   real-estate.  This macro should be used in conjunction with the
   BEGIN_NAMESPACE macro.
   
   \see BEGIN_NAMESPACE
*/
#ifndef END_NAMESPACE
#define END_NAMESPACE(x) }
#endif

/**\def BEGIN_NAMESPACE(x)

   \brief A convenient macro for generating namespace declarations

   This macro provides a convenient approach for defining namespaces
   in C++ headers and source files.  Using this macro ensures that
   indendation for namespaces do not clutter or eat up screen
   real-estate.  Here is an example of how to use this macro:

   \code

   #include "Utilities.h"

   BEGIN_NAMESPACE(muse);

   // Rest of the C++ class definitions etc. go here.
   
   END_NAMESPACE(muse);
   
   \endcode

   \see END_NAMESPACE
*/
#ifndef BEGIN_NAMESPACE
#define BEGIN_NAMESPACE(x) namespace x {
#endif

/** \def getTimeStamp
    
    \brief Get's the file modification timestamp for a given file
    name.
    
    This method provides a portable (to Windows and Linux/Unix)
    implementation for a helper method to obtain the modification
    timestamp for a given file.
    
    \param[in] fileName The file name (with full path) for which the
    modification time stamp is desired.

    \return A string containing the last modification time stamp for
    the given file.
*/
std::string getTimeStamp(const std::string& fileName);

/** \def getSystemTime
    
    \brief Returns the string representation of the supplied time data
    structure.
    
    This method provides a portable (to Windows and Linux/Unix)
    implementation for a helper method to obtain the string
    representation of a given encoded time_t datastructure.
    
    \param[in] codedTime The encoded time_t data structure to be
    converted to a string representation.  If this parameter is NULL,
    then the current system time is converted to a string and filled
    into the supplied buffer.

    \return A string containing a human readable format of the time.
*/
std::string getSystemTime(const time_t *codedTime = NULL);

/** \def ctime_s Macro to define ctime_s if not defined.

    This macro provides a replacement for the \c ctime_s function
	defined in Windows but is absent in Unix/Linux. This macro 
    simply defines \c ctime_s as \c ctime_r in Unix and Linux.
*/
#if (!defined(_WINDOWS) && !defined(ctime_s))
#define ctime_s(buffer, size, time) ctime_r(time, buffer);
#endif

/**\def UNUSED_PARAM(x)

   \brief A convenient macro for specifying that a parameter to a
   method is not used.

   This macro provides a convenient approach for tagging unused
   parameters to avoid compiler warnings.  These are only meant to be
   used for parameters that are really not used (possibly in a base
   class or a derived class) but are necessary for API compatibility
   and documentation purposes.  Here is an example of how to use this
   macro:

   \code

    virtual void garbageCollectionDone(const muse::Time& gvt) {
	UNUSED_PARAM(gvt);
	// Possibly more code goes here.
    }

   \endcode
*/
#define UNUSED_PARAM(x) (void) x

/**
 * Constant to convert 1-degree latitude to distances in miles.
 */
const double MILES_PER_LAT = 69.04677;

/** Helper method to remove leading and trainling whitespaces from a
 * given string.
 *
 * \param[in] str The string to be trimmed.
 *
 * \return The trimmed version of str.
 */
extern std::string trim(const std::string& str);

/** Convert the given latitude, longitude to x, y coordinates given a
    dimension.

    \param[in]  latitude  The latitude to be convereted.
    \param[in]  longitude The latitude to be convereted.
    \param[out] x         The x-coordinate for the longitude.
    \param[out] y         The y-coordinate for the latitude.
    \param[in]  size      The dimension for the size
    \param[in] wrap       Wrap the latitude and longitude.
*/
extern void getXYValues(double latitude, double longitude,
			int &x, int &y, const int size,
			const bool wrap = true);


/** Convert the given x, y coordinates to latitude, longitude

    \param[in]  x         The x-coordinate for the longitude.
    \param[in]  y         The y-coordinate for the latitude.
    \param[in]  MapSize   The map size corresponding to 180/360.
    \param[out] latitude  The latitude to be convereted.
    \param[out] longitude The latitude to be convereted.
*/
extern void getLatLon(const int x, const int y, const int MapSize,
		      double &lat, double& lon);


/** Convert distance at a given latitude to corresponding longitude value.

    \param[in] miles The distance in miles to be convereted to
    longitude.

    \param[in] latitude The latitude at which the distance is to be
    convereted.

    \return The longitude difference at the given latitude.
*/
extern double toLongitude(const double miles, const double latitude);

/** The distance in miles between a given pair of points based on
    Haversine's formula.

    \param[in] latitude1 The latitude for the first point.
    \param[in] longitude1 The longitude for the first point.
    \param[in] longitude2 The longitude for the first point.
    \param[in] latitude2 The latitude for the second point.

    \return The distance between 2 points based on haversine's formula.
*/
extern double getDistance(double latitude1, double longitude1,
                          double latitude2, double longitude2);

/** Checks to see if val3 is in between val1 and val2

    \param[in] val1 One end of the range to check.

    \param[in] val2 Otehr end of the range to check.

    \param[in] val3 Value to be checked to see if it is in between
    val1 and val2.

    \return This method returns true if val3 is in between val1
    and val2 (val1 may be lesser or greater than val2).
*/
extern bool inBetween(const double val1, const double val2,
                      const double val3);

/** Helper method to find perpendicular intersection between a
    given entrance and a segment of a way.

    \param[in] entLat Latitude of the fixed point (or entrance point
    of a given building) from where perpendicular distance is to be
    computed.

    \param[in] entLat Latitude of the second point (or entrance point
    of a given building) from where perpendicular distance is to be
    computed.
    
    \param[in] node1Lat The latitude of the first node for the given
    way/line segment.

    \param[in] node1Lon The longitude of the first node for the given
    way/line segment.

    \param[in] node2Lat The latitude of the second node for the given
    way/line segment.

    \param[in] node2Lon The longitude of the second node for the given
    way/line segment.

    \param[out] interLat The latitude of the intersection point for
    the fixed point on the given way/line.

    \param[out] interLon The longitude of the intersection point for
    the fixed point on the given way/line.

    \return This method returns true if a valid intersection was
    found.
*/
bool findPerpendicularIntersection(const double entLat,   const double entLon,
                                   const double node1Lat, const double node1Lon,
                                   const double node2Lat, const double node2Lon,
                                   double& interLat, double& interLon);

/** Helper method to find a new point along a given line at a given
    distance away from node1.

    \note If the distance between node1 and node2 is smaller than
    dist, then this method will generate points beyond node2 but on
    the straightline between node1 and node2.

    \param[in] entLat Latitude of the fixed point (or entrance point
    of a given building) from where perpendicular distance is to be
    computed.

    \param[in] entLat Latitude of the second point (or entrance point
    of a given building) from where perpendicular distance is to be
    computed.
    
    \param[in] node1Lat The latitude of the first node for the given
    way/line segment.

    \param[in] node1Lon The longitude of the first node for the given
    way/line segment.

    \param[in] node2Lat The latitude of the second node for the given
    way/line segment.

    \param[in] node2Lon The longitude of the second node for the given
    way/line segment.

    \param[in] dist The distance from node1Lat and node1Lon towards
    node2Lat and node2Lon where the point is to be located.
    
    \param[out] pointLat The latitude of the new point at a given
    distance from node1 towards node2.

    \param[out] pointLon The longitude of the new point at a given
    distance from node1 towards node2.

    \param[in] offset If this flag is true then node1Lat and node2Lon
    is subtracted from the point's latitude and longitude to provide
    offset values rather than absolute values.
    
    \return This method returns true if a valid intersection was
    found.
*/
void getPoint(const double node1Lat, const double node1Lon,
              const double node2Lat, const double node2Lon,
              const double dist, double& pointLat, double& pointLon,
              const bool offset = false);

/** Handy macro to convert radians to degrees */
constexpr double TO_DEGREES(const double rads) {
    return ((rads) * 180.0 / M_PI);
}

/** Handy macro to convert degrees to radians */
constexpr double TO_RADIANS(const double angle) {
    return ((angle) * M_PI / 180.0);
}

#endif
