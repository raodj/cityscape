#ifndef OSM_WAY_H
#define OSM_WAY_H

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
#include <iostream>
#include "OsmTags.h"
#include "OsmNode.h"

// The list of OSM Node-Ids associated with a OSM-way
using NodeList = std::vector<long>;

/** A simple class to encapsulate information about a way contained
    within the bounds of an OSM section.  A way represents a
    highway/road/street as a series of OsmNode entries.  This class
    reflects the XML structure in that the nodes associated with the
    way are stored as ids.  For processing, the IDs are used to obtain
    the actual OsmNode object.  In addition, a way can have attributes
    indicated by &lt;tag/&gt; elements in the XML.  The tags are
    managed by the OsmTags base class.
*/
class OsmWay : public OsmTags {
    /// Convenience method to print raw way information.  See print() method.
    friend std::ostream& operator<<(std::ostream&, const OsmWay&);
public:
    /** Convenience (and default) constructor to create a way

        This constructor can be used to create a way given its id
        information.

        \param[in] id The OSM-id value associated with this way
        object.
    */
    OsmWay(const long id = -1);

    /** The destructor.

        This is just a place holder as the destructor does not have
        any special tasks to perform (because this class does not
        directly use any dynamic memory).
     */
    ~OsmWay();

    /** Add a node entry to this way.

        This is a convenience method that can be usedd to add a given
        node entry to this way.  Note that only the ID of the node is
        recorded and not the full node object.  This is done to
        minimize the memory footprint of an way object.

        \param[in] id The OSM-id of the node to be added.  No checks
        are performed on this value.
    */
    void addNode(const long id);

    /** Print detailed way information.

        This method is a convenience method to print detailed
        information about a way, including information about each node
        associated with this way.

        \param[in] nodes A hash map (key is node ID) containing node
        information to be used for printing details.  Note that this
        map will contain more nodes than that are in this way.

        \param[out] os The output stream to where the data is to be
        written.

        \see OsmSection::printWays
     */
    void print(const NodeMap& nodes, std::ostream& os = std::cout) const;

    /** Obtain the list of node IDs associated with this way.

        \return The list of Node IDs associated with this node.
    */
    const NodeList& getNodeIDs() const { return nodeList; }

    /** Obtain the OSM-ID associated with this way.

        \return The OSM-ID for this node (set when this object was
        instantiated).
    */
    long getID() const { return id; }
    
protected:
    // Currently this class does not have protected members

private:
    /**  The unique OSM-id value associated with this way.  Typically
        this value is set once in the constructor and is never changed
        during the life time of this object.
    */
    long id;

    /** The OSM-IDs of the list of nodes associated with this way.
        Entries are added via the addNode method. Typically, entries
        are added in the same order as they are in the XML.  The order
        of entries important.  Note that this list only maintain the
        IDs and not the actual OsmNode objects.  This design is used
        to minimize the memory footprint and to reflect the XML
        structure.

        \note Order of entries in this list is important to ensure
        that the raods/streets are correctly represented.
    */
    NodeList nodeList;
};

/** A vector containing a list of Way objects.  This is especially
    useful when processing all streets in a given OsmSection.  This
    list is actually encapsulated into the OsmSection.  However, it is
    defined here because the list may be used in different contexts
    for processing.
*/
using WayList = std::vector<OsmWay>;

#endif
