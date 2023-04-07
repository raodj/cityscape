#ifndef BUILDING_H
#define BUILDING_H

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
#include "PUMSHousehold.h"

/** A simple class to encapsulate node information extracted from
    parse XML file.  This class essentially encapsulates the following
    information:

    \code
    <way id="636473401" version="1">
        <nd ref="6001935464"/>
        <nd ref="6001935455"/>
        <nd ref="6001935457"/>
        <nd ref="6001935462"/>
        <nd ref="6001935461"/>
        <nd ref="6001935468"/>
        <nd ref="6001935464"/>
        <tag k="building" v="yes"/>
    </way>
    \endcode
   
    Each building is identified by a unique id number.  The dimensions
    of the building are indicated by the node references.  These nodes
    are used to determine the dimensions of the building and its
    square footage.
*/
class Building {
public:
    /** Convenience method to create a building from a ring of points.

        \param[in] ring The ring with the points associated with this
        building.

        \param[in] levels The number of levels associated with this
        building (assuming levels is available).

        \param[in] population The initial number of people living at
        this building.

        \param[in] attributes The attributes (if any) to be associated
        with this building.
    */
    static Building create(const Ring& ring, int id, int levels = -1,
                           int population = 0, int attributes = 0);

    /** Convenience method to create a building from a given set of
        information.

        \param[in] id The ID to be set for this building.

        \param[in] sqFootage The square footage for this building.

        \param[in] wayID The way associated with this building.

        \param[in] wayLat The latitude for the entry for this building.

        \param[in] wayLon The longitude for the entry for this building

        \param[in] topLat The latitude for the top-left corner of the
        building.
        
        \param[in] topLon The longitude for the top-left corner for
        the building.

        \param[in] botLat The latitude for the bottom-right corner of the
        building.
        
        \param[in] botLon The longitude for the bottom-right corner
        for the building.

        \param[in] popRingID The population ring ID that logically
        contains this building.
        
        \param[in] levels The number of levels associated with this
        building (assuming levels is available).

        \param[in] population The initial number of people living at
        this building.

        \param[in] pumaID The PUMA area ID associated with this building.
    */
    static Building create(int id, int sqFootage, long wayID,
                           double wayLat, double wayLon, double topLat,
                           double topLon, double botLat, double botLon,
                           int popRingID, bool isHome = true,
                           int levels = 1, int population = 0,
                           int pumaID = -1);
    
    /** Write this Building to a given output stream.

        This is a helper method that is used to write the information
        associated with this building to a given output stream.

        \param[out] os The output stream to where the information
        about this building is to be written.

        \param[in] writeHeader If this flag is true then a simple
        comment with the order of the fields is written for future
        reference.
        
        \param[in] delim An optional delimiter between each value
        associated with this node.
    */
    void write(std::ostream& os, const bool writeHeader = false,
               const std::string& delim = " ") const;

    /** Write households associated with this building to a given
        output stream.

        This is a helper method that is used to write the information
        associated with households to a given output stream.

        \param[out] os The output stream to where the information
        about this building is to be written.

        \param[in] writeHeader If this flag is true then a header is
        written before the actual data.
        
        \param[in] delim An optional delimiter between each value
        associated with this node.
    */
    void writeHouseholds(std::ostream& os, bool writeHeader = false,
                         const std::string& delim = " ") const;

    /** Return information about the building based on a given key
        column.

        \param[in] key The key column for which information is to be
        returned. Valid keys are: "people", "households", "avg_income"

        \return The information about this building.
    */
    int getInfo(const std::string& key) const;
    
    /** Load the data for this building from a given input stream.

        This is a helper method that is used to read information for
        this building from a given input stream.  This method assumes that
        the data has been written by a prior call to the write method
        in this class.

        \param[in,out] is The input stream with 1 line of data from
        where the information for this building is to be read.
    */
    void read(std::istream& is);

    /** Add information about an household living in this building.

        \param[in] hdl The household to be added to this building.
        The household class encapsulates information about the
        household to be added.  The household information consists of
        a set of people (when serialized, people is written separated
        by semicolons).

        \param[in] people The number of people in the household being
        added.
        
        \see PUMSHousehold::getInfo()
    */
    void addHousehold(const PUMSHousehold& hld, const int people,
                      const std::string& peopleInfo);

    /** Obtain the total square footage of all levels in this building.

        \note The minimum number of levels is set to 1 in this method
        to handle scenarios where level for a building is not
        explicitly set.  This ensures that bulidings never get a
        negative or zero area.
        
        \return The total square footage of all the levels in this
        building.
    */
    inline double getArea() const {
        return sqFootage * std::max(1, levels);
    }

    /** The unique id set for this node.  This is the same id value
        that was in the OSM XML file */
    unsigned int id = 0;

    /** The number of people living at this building */
    int population;

    /** The number of levels in this building.  Some buildings have
        levels set for them in the XML via the following tag:
        
        <tag k="building:levels" v="2"/>
    */
    int levels;

    /** The square footage of the area associated with this buliding.
        This value is computed from the original polygon associated
        with the building (and not just its bounds).
    */
    int sqFootage;
    
    /** Any custom attributes associated with this building */
    int attributes;

    /** The ID (obtained from OSM XML) for the street associated with
        this building. This street information is used for routing
        traffic to/from this building. */
    long wayID;

    /** The latitude on the way where the pick-up/drop-off location
        for this building is set */
    double wayLat;

    /** The longitude on the way where the pick-up/drop-off location
        for this building is set */
    double wayLon;
    
    /** The latitude value associated with top-left corner for this
        building */
    double topLat = 0;

    /** Flag to indicate if the building is a residential space. */
    bool isHome;
    
    /** The longitude value associated with the top-left corner for
        this building */
    double topLon = 0;

    /** The latitude value associated with bottom-right corner for
        this building */
    double botLat = 0;

    /** The longitude value associated with the bottom-right corner for
        this building */
    double botLon = 0;

    /** This integer contains the PUMA area ID associated with this
        building.  This information is used to determine family and
        age of people living in this building.
    */
    int pumaID;

    /** This vector contains PUMS information of the different
        households assigned to this building.  Note that for large
        buildings the number of households can be large. Each
        household consists of a set of semicolon separated people.
    */
    std::vector<PUMSHousehold> households;
};

#endif
