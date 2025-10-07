#ifndef UTILITIES_CPP
#define UTILITIES_CPP

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

#include "Utilities.h"
#include <sys/types.h>
#include <sys/stat.h>

std::string
getTimeStamp(const std::string& fileName) {
    if (fileName.empty()) {
        // Nothing further to be done here.
        return "Empty path";
    }
    // The follwing structure will contain the file information.
    struct stat fileInfo;
    int returnValue = 0;
#ifdef _WINDOWS
    returnValue = _stat(fileName.c_str(), &fileInfo);
#else
    // The only difference between windows and Linux is the extra "_"
    // at the beginning of stat() method.
    returnValue = stat(fileName.c_str(), &fileInfo);
#endif
    // Check to ensure there were no errors.  If there were errors
    // exit immediately.
    if (returnValue == -1) {
        // O!o! there was an error.
        return "Invalid path";
    }
    // Convert the last modification time to string and return it back
    // to the caller.
    return getSystemTime(&fileInfo.st_mtime);
}

// Return given time or current-system-time as a string
std::string getSystemTime(const time_t *encodedTime) {
    // Get current system time.
    time_t timeToConv = time(NULL);
    if (encodedTime != NULL) {
        // We don't have a valid time supplied, then override system time.
        timeToConv = *encodedTime;
    }
    // Convert the time.
    char buffer[128];
    ctime_s(buffer, 127, &timeToConv);
    // Return the buffer back to the caller
    return buffer;
}

// Remove leading and trailing white spaces
std::string
trim(const std::string& str) {
    std::string::size_type pos1 = str.find_first_not_of("\r\n\t ");
    if (pos1 == std::string::npos) {
        return "";
    }
    std::string::size_type pos2 = str.find_last_not_of("\r\n\t ");
    return str.substr(pos1 == std::string::npos ? 0 : pos1, 
                      pos2 == std::string::npos ? str.length() - 1 :
                      pos2 - pos1 + 1);
}

// Convert latitude & longitude to x & y coordinates on a map
// (mercator projection assumed) of given "size"
void getXYValues(double latitude, double longitude,
                 int &x, int &y, const int size, const bool wrap) {
    // First normalize the latitude and a longitude and normalises the
    // values so they can be scaled to any size
    if ((wrap) && (longitude > 180)) {
        longitude -= 360;
    }
    longitude /= 360;
    longitude += 0.5;
    if (latitude <= -90) {
        latitude = 1;
    } else if (latitude >= 90) {
        latitude = 0;
    } else {
        latitude *= (M_PI / 180.0); // Conver to radians
        latitude = 0.5 - ((log(tan((M_PI / 4) + (0.5 * latitude)))/M_PI)/2.0);
    }
    
    // Now calculate the pixel value using normalized latitude and
    // longitude.
    x = (int) (longitude * size);
    y = (int) (latitude  * size);
}

// Convert distance to longitude at a given latitude -- note due to
// spherical surface of earth longitude values will be different at
// different latitudes.
double toLongitude(const double miles, const double latitude) {
    return miles / (cos(latitude * M_PI / 180.0) * MILES_PER_LAT);
}

// This is the converse of getXYValues method -- given x, y on map of
// given "MapSize", it computes latitude, longitude values.
void getLatLon(const int x, const int y, const int MapSize,
               double &lat, double& lon) { 
    lat = lon = 0;
    lat = (double) ((MapSize / 2) - y) / (MapSize / (2 * M_PI));
    lon = (double) x / MapSize;
    // Put X back to the way it was (longitude)
    lon -= 0.5;
    lon *= 360;
    // Y is going to be trickier! (latitude)
    lat = TO_DEGREES(atanl(sinhl(lat)));
}

// Get the distance between 2 points -- this method is good for short
// distances (say less than 100 miles) but has some error (don't know
// how much error) for large distances (say for 1000s of miles)
double
getDistance(double latitude1, double longitude1,
	    double latitude2, double longitude2)  {
    const double latDelta = TO_RADIANS(latitude1  - latitude2);
    const double lonDelta = TO_RADIANS(longitude1 - longitude2);

    const double sinLatDelta = sin(latDelta / 2);
    const double sinLonDelta = sin(lonDelta / 2);
    
    // Convert the delta's to miles using the Haversine formula
    double a = (sinLatDelta * sinLatDelta) +
        (cos(TO_RADIANS(latitude1)) * cos(TO_RADIANS(latitude2)) *
         (sinLonDelta * sinLonDelta));
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    // Now compute flat distance in miles (3958.75587 => radius of
    // earth)
    double distance = 3958.75587 * c;
    
    // Now return the distance in miles back to the caller.
    return distance;        
}

// A simple method to detect if val3 is between val1 and val2
constexpr double epsilon = 0.001;
bool inBetween(const double val1, const double val2,
               const double val3) {
    const double min = std::min(val1, val2) - epsilon;
    const double max = std::max(val1, val2) + epsilon;
    return ((min <= val3) && (val3 <= max));    
}

// Find perpendicular distance between an entrance and 2 nodes
bool findPerpendicularIntersection(const double entLat,   const double entLon,
                                   const double node1Lat, const double node1Lon,
                                   const double node2Lat, const double node2Lon,
                                   double& interLat, double& interLon) {
    // Compute perpendicular distance from the entrance to this
    // segment.  First calculate common term delta latitude (dLat)
    // and delta longitude (dLon).
    const double dLat = (node2Lat - node1Lat);
    const double dLon = (node2Lon - node1Lon);
    // Compute intercept factor.
    const double k = (dLat * (entLon - node1Lon) - dLon * (entLat - node1Lat)) /
        (pow(dLat, 2) + pow(dLon, 2));
    // Now compute the intercept coordinates on the way's segment
    interLon = entLon - (k * dLat);
    interLat = entLat + (k * dLon);
    // Check if the coordinates are within the line segment of choice
    if (inBetween(node1Lat, node2Lat, interLat) &&
        inBetween(node1Lon, node2Lon, interLon)) {
        // The intersection is within bounds of the line segment. So
        // it is acceptable.
        return true;
    }
    // A valid intercept could not be found
    return false;
}

// Compute lat, lon of a point on a given line that is "dist" miles
// away from node1 (towards node2).
void getPoint(const double node1Lat, const double node1Lon,
              const double node2Lat, const double node2Lon,
              const double dist, double& pointLat, double& pointLon,
              const bool offset) {
    // This method first computes the average lat, lon change per mile
    // based the slope of the line.
    const double totDist = getDistance(node1Lat, node1Lon, node2Lat, node2Lon);
    // Using totalDist map the provided distance
    pointLat = node1Lat + (node2Lat - node1Lat) / totDist * dist;
    pointLon = node1Lon + (node2Lon - node1Lon) / totDist * dist;
    // Convert to offsets if requested
    if (offset) {
        pointLat -= node1Lat;
        pointLon -= node1Lon;
    }
}

std::vector<std::string>
split(std::string str, const std::string& delimiter) {
    std::vector<std::string> wordList;
    while (!str.empty()) {
        size_t delim = str.find_first_of(delimiter);
        delim        = (delim != std::string::npos ? delim : str.size());
        // Get the next token and add it to return list
        std::string word = str.substr(0, delim);
        wordList.push_back(word);
        // Remove the word we have already added
        str = (delim < str.size() ? str.substr(delim + 1) : "");
    }
    return wordList;
}

std::string getColumn(const std::string& line, const int column,
                      const char delim) {
    int currCol = 0, startPos = 0;
    while (currCol < column) {
        startPos = line.find(delim, startPos + 1);
        currCol++;
    }
    int nextTab = line.find(delim, startPos + 1);
    return line.substr(startPos + 1, nextTab - startPos - 1);
}

std::string getColumn(const std::string& line,   const int column,
                      const std::string& defVal, const char delim) {
    const std::string val = getColumn(line, column, delim);
    return (val.empty() ? defVal : val);
}

#endif
