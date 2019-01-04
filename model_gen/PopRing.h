#ifndef POP_RING_H
#define POP_RING_H

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

#include "Ring.h"
#include <cmath>

/** A simple class to encapsulate information extracted from a model
    text file regarding population rings.  The ring information is
    stored in a model file in the following format:

    # Rng ID ringID shpaeID pop num_vert tl_lat tl_lon br_lat br_lon num_bld num_homes home_tot_sqft
*/
class PopRing : public Ring {
public:
    /** The unique id set for this ring.  IDs are sequential index
        numbers starting with zero. */
    long id = -1;

    /** A ring id set for this ring. This value is typically not
        used */
    long ringID = 0;

    /** A shape id set for this ring. This value is typically not used
        but is included for copleteness/future-use. */
    long shapeID = 0;
    
    /** The top-left latitude value associated with this ring */
    double tlLat = 0;

    /** The top-left longitude value associated for this ring */
    double tlLon = 0;

    /** The top-left latitude value associated with this ring */
    double brLat = 0;

    /** The top-left longitude value associated for this ring */
    double brLon = 0;

    /** The total number of buildings in this ring */
    int numBuildings = 0;

    /** The total square footage of all the buildings in this ring */
    long bldsTotSqFt = 0;
    
    /** The subset of buildings that are deemed as homes in this ring */
    int numHomes = 0;

    /** The total square footage of homes in this ring */
    long homesTotSqFt = 0;

    /** Determine the width of this population ring.

        \return The width (i.e., difference in longitude) for this
        ring.  The return value is always positive
     */
    const double getWidth() const { return std::abs(tlLon - brLon); }

    /** Determine the height of this population ring.

        \return The height (i.e., difference in latitude) for this
        ring.  The return value is always positive
     */
    const double getHeight() const { return std::abs(tlLat - brLat); }

    /** A simple method to check if a population ring is deemed valid.
        Currently, we have a simple assumption that if the id is
        positive, then this ring is valid.

        \return This method returns true if the population ring is
        deemed positive.
    */
    const bool isValid() const { return id >= 0; }
    
    /** Load the data for this node from a given input stream.

        This is a helper method that is used to read information for
        this node from a given input stream.  This method assumes that
        the data has been written by a prior call to the write method
        in this class.

        \param[in,out] is The input stream from where the information
        for this node is to be read.
    */
    void read(std::istream& is);
};

#endif
