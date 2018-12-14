#ifndef PATH_SEGMENT_H
#define PATH_SEGMENT_H

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

/** A simple class to encapsulate information about a segment in the
    path to be computed by the PathFinder.
*/
class PathSegment {
    friend std::ostream& operator<<(std::ostream&, const PathSegment&);
public:
    /** The way ID associated with this path segment. */
    long wayID;

    /** The node ID in the way associated with this path segment */
    long nodeID;

    /** The building ID if any for this path segement. Note that
        either the nodeID or building ID is set for a path segement
        (but not both)
    */
    long buildingID;

    /** The distance (in miles) from the previous path segment (if
        any) associated with this path segment. */
    double distance;
};

#endif
