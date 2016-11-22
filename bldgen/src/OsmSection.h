#ifndef OSM_SECTION_H
#define OSM_SECTION_H

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

// Note: This file requires including "Boost.h" in .cpp files.  This
// enables correct use of precompiled headers to keep compiles fast.

#include <iostream>
#include <string>

// Nested/encapsulated objects used by this class
#include "OsmNode.h"
#include "OsmWay.h"

/** The top level class to encapsulate a section of data from Open
    Street Maps (OSM).  This class essentially loads data from a given
    OSM XML file downloaded from openstreetmap.org.  For example:

    $ wget "http://openstreetmap.org/api/0.6/map?bbox=-84.75,39.5,-84.733,39.515" -O "miami.osm"

    The bounds of the region associated with this section of the data
    is stored in the minLat, minLon, maxLat, and maxLon instance
    variables in this class.
*/
class OsmSection {
public:
    /** The default (and only) constructor.

        Merely initializes the various instance variables to their
        default initial value(s).
     */
    OsmSection();

    /** The destructor.

        This is just a place holder as the destructor does not have
        any special tasks to perform (because this class does not
        directly use any dynamic memory).
    */
    ~OsmSection();

    /** Method to load data from a given OSM XML data file.

        This method must be used to load data for nodes and ways from
        a given OSM XML file.

        \param[in] filePath The path to the OSM XML file from where
        the data is to be loaded into this class.
    */
    void load(const std::string& filePath);

    /** Convenience method to print all the ways data in this object.

        This method prints all the ways data along with nodes.  The
        output is useful for troubleshooting/debugging purposes.

        \param[out] os The output stream to where the data is to be
        written.
    */
    void printWays(std::ostream& os = std::cout) const;

    /** Obtain the node object associated with a given node ID.

        This is a convenience method that can be used to obtain the
        OsmNode object associated with a given OSM-ID.

        \param[in] id The OSM-ID of the node to be returned by this
        method.
        
        \return The OsmNode object associated with this given ID.  If
        the object is not found, then this method returns an
        OsmNode::InvalidNode as the result.
    */
    const OsmNode& getNode(long id) const;
    
    /** Obtain the set of nodes in this OSM section.
        
        This method can be used to obtain the set of nodes associated
        with this OSM section.  The nodes are loaded from a specific
        OSM XML file via the load method in this class.
        
        \return A unordered_map containing the set of nodes.
    */
    const NodeMap& getNodes() const { return nodes; }

    /** Obtain the list of ways (roads, streets, etc.) in this OSM
        section.
        
        This method can be used to obtain the set of ways (roads,
        streets, etc.) associated with this OSM section.  The ways
        contain the list of nodes that describe them.  The information
        about nodes can be obtained via call to getNodes method in
        this class.

        \return A vector containing the list of ways in this OSM
        section.
    */    
    const WayList& getWays() const { return ways; }


    /** Obtain the bounds set for this OSM section.  This method
        returns the bounds that was loaded from the XML data
        associated with this section.

        \param[out] minLat The minimum latitude value for this section.

        \param[out] minLon The minimum longitude value for this section.

        \param[out] maxLat The maximum latitude value for this
        section.

        \param[out] maxLon The maximum longitude value for this
        section.
    */
    void getBounds(double& minLat, double& minLon,
                   double& maxLat, double& maxLon) const;

protected:
    /** A helper method to create a OsmNode object using data from the
        correspoding "node" XML element.  This method is called from
        the load method in this class to create a OsmNode object.
        This method extracts the id, latitude, longitude, and tags
        from the "node" XML element to create the object.

        \param[in] node The XML element associated with the "node"
        object from the property_tree.

        \return The OsmNode object containing the node data.
    */
    OsmNode createNode(const boost::property_tree::ptree& node);

    /** A helper method to create a OsmWay object using data from the
        correspoding "way" XML element.  This method is called from
        the load method in this class to create a OsmWay object.  This
        method extracts the id, nd, and tag entries the "node" XML
        element to create the object.

        \param[in] node The XML element associated with the "way"
        object from the property_tree.

        \return The OsmWay object containing the way data.
    */
    OsmWay createWay(const boost::property_tree::ptree& node);
    
    /** Utility method to set the bounds of this section from the
        corresponding "bounds" node in XML data.

        This method is invoked from the load() method to set the
        logical bounds associated with this OSM section.  This method
        extracts the data from the "bounds"'s attribute node in the
        XML which is in the form:
        
        \code
        <bounds minlat="39.5000000" minlon="-84.7500000" maxlat="39.5150000" maxlon="-84.7330000"/>
        \endcode

        \param[in] boundsAttrNode The bounds node in the XML property
        tree from where the bounds are to be extracted.  This value is
        typically obtained from the root as:
        pt.get_child("osm.bounds.<xmlattr>")
    */
    void setBounds(const boost::property_tree::ptree& boundsAttrNode);
    
    /** This is just a helper method to recursively print the contents
        of a boost::property_ptree::ptree to help understanding the
        XML structure and for troubleshooting.

        \param[in] node The top-level node from where all values and
        children (includes attributes) are to be printed.

        \param[out] os The output stream to where the contents is to
        be printed.

        \param[in] indent Initial indendation to be used.  Each nested
        level is indented by 2 more spaces.
    */
    void print(const boost::property_tree::ptree& node,
               std::ostream& os = std::cout,
               const std::string& indent = "  ") const;

private:
    /** The minimum latitude, i.e., bottom latitude of the logical
        bounding box for which this object encapsulates data.  This
        information is extracted from the XML and stored in this
        instance variable. See setBounds method in this class.
    */
    double minLat;

    /** The minimum longitude, i.e., right-most longitude of the
        logical bounding box for which this object encapsulates data.
        This information is extracted from the XML and stored in this
        instance variable. See setBounds method in this class.
    */    
    double minLon;

    /** The maximum latitude, i.e., top latitude of the logical
        bounding box for which this object encapsulates data.  This
        information is extracted from the XML and stored in this
        instance variable. See setBounds method in this class.
    */    
    double maxLat;

     /** The maximum longitude, i.e., left-most longitude of the
        logical bounding box for which this object encapsulates data.
        This information is extracted from the XML and stored in this
        instance variable. See setBounds method in this class.
    */    
    double maxLon;

    /** The nodes in the XML section associated with this OsmSection.
        The nodes are stored as a a hash map to quickly look-up a
        OsmNode entry given its OSM-id value.  This is especially
        useful when processing OsmWay objects that only store the
        OSM-id of the nodes associated with them. This map is actually
        encapsulated into the OsmSection.  However, it is defined here
        because both OsmSection and OsmWay use this map for
        processing.

        \note Entries are added via the load method.
    */
    NodeMap nodes;

    /** The list of ways (roads/streets etc.) in this section.

        This list maintains the set of roads in this section of OSM.
        The list of entries are typically present in the same order in
        which they were present in the XML data file.

        \note Entries are added in the load method.
    */
    WayList ways;
};

#endif
