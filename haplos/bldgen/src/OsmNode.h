#ifndef OSM_NODE_H
#define OSM_NODE_H

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

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "OsmTags.h"
#include "Utils.h"

/** A simple class to encapsulate information about a node contained
    within the bounds of an OSM section.  A node is just a single
    point or geolocation represented by its latitude and longitude
    value.  In addition, nodes can have attributes indicated by
    &lt;tag/&gt; elements in the XML.  The tags are managed by the
    OsmTags base class.
*/
class OsmNode : public OsmTags {
    /** Convenience method to print node information.

        \param[out] os The output stream to where the data is to be
        written.

        \param[in] node The node whose information is to be printed.

        \return The output stream passed in as the parameter.        
    */
    friend std::ostream& operator<<(std::ostream& os, const OsmNode& node);

 public:
    /** Convenience (and default) constructor to create a node.

        This constructor can be used to create a node given its key
        information.

        \param[in] id The OSM-id value associated with this node.

        \param[in] latitude The latitude value associated with this
        node.  The latitude is typically in the range -90 < latitude <
        90.

        \param[in] longitude The longitude value associated with this
        node.  The longitude is typically in the range -180 <
        longitude < 180.
    */
    OsmNode(const long id = -1, const double latitude = 0.0,
            const double longitude = 0.0);

    /** The destructor.

        This is just a place holder as the destructor does not have
        any special tasks to perform (because this class does not
        directly use any dynamic memory).
     */
    ~OsmNode();

    /** Obtain the OSM-ID associated with this node.

        \return The OSM-ID for this node (set when this object was
        instantiated).
    */
    inline long getID() const { return id; }

    /** Obtain the latitude and longitude value for this node.

        This method can be used to obtain the latitude and longitude
        values associated with this node.  This is the value that was
        loaded from the XML file associated with this node.

        \return The <latitude, longitude> pair of values associated
        with this node.
     */
    inline Location getLocation() const {
        return Location(latitude, longitude);
    }

    /** Return the great circle distance from this node to another.

        This method computes the distance between this node and.  The
        distance between two nodes is computed using Haversine
        formula: https://en.wikipedia.org/wiki/Haversine_formula

        \param[in] other The other node to which the distance is to be
        computed.

        \return The distance to the other node in miles.

        \see Utils::getDistance
    */
    double operator-(const OsmNode& other) const;

    /** A constant object used to refer to an invalid node.

        This is a convenience global constant that is used to refer to
        an invalid node.  This constant is used as return value from
        methods that return references to OsmNode objects.
    */
    static const OsmNode InvalidNode;
    
protected:
    // Currently this class does not have protected members

private:
    /**  The unique OSM-id value associated with this node.  Typically
        this value is set once in the constructor and is never changed
        during the life time of this object.
    */
    long id;
    
    /** The latitude value associated with this node.  This value is
        in the range: -90 < latitude < 90.  Typically this value is
        set once in the constructor and is never changed during the
        life time of this object.
    */
    double latitude;

    /** The longitude value associated with this node.  This value is
        in the range: -180 < longitude < 180.  Typically this value is
        set once in the constructor and is never changed during the
        life time of this object.
    */    
    double longitude;
};

/** A hash map to quickly look-up a OsmNode entry given its OSM-id
    value.  This is especially useful when processing OsmWay objects
    that only store the OSM-id of the nodes associated with them. This
    map is actually encapsulated into the OsmSection.  However, it is
    defined here because both OsmSection and OsmWay use this map for
    processing.
*/
using NodeMap = std::unordered_map<long, OsmNode>;

#endif
