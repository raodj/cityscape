#ifndef OSM_SECTION_CPP
#define OSM_SECTION_CPP

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

#include "Boost.h"
#include "OsmSection.h"

// Using boost property tree
using namespace boost::property_tree;
using namespace boost::algorithm;

// The default (and only) constructor.
OsmSection::OsmSection() {
    // Initialize the bounds to zero range.
    minLat = 0;
    maxLat = 0;
    minLon = 0;
    maxLon = 0;
}

OsmSection::~OsmSection() {
    // Nothing else to be done.
}

// This is just a helper method to print the contents of the given
// boost::property_tree::ptree node (and its children).  The output is
// merely used for troubleshooting purposes.
void
OsmSection::print(const ptree& node, std::ostream& os,
                 const std::string& indent) const {
    std::string data = node.data();
    os << data;  // if there is some data to print?
    // Print all of the children.  The formatting of this method is
    // not great but the data is correct and useful for
    // troubleshooting
    const std::string subIndent = indent + "  ";
    for (ptree::const_iterator child = node.begin(); (child != node.end());
         child++) {
        os << indent << "<" << child->first << ">";
        print(child->second, os, subIndent);
        os << indent << "</" << child->first << ">\n";
    }
}

void
OsmSection::setBounds(const boost::property_tree::ptree& bounds) {
    minLat = std::stod(bounds.get_child("minlat").data());
    minLon = std::stod(bounds.get_child("minlon").data());
    maxLat = std::stod(bounds.get_child("maxlat").data());
    maxLon = std::stod(bounds.get_child("maxlon").data());
}

void
OsmSection::getBounds(double& minLat, double& minLon,
                      double& maxLat, double& maxLon) const {
    minLat = this->minLat;
    minLon = this->minLon;
    maxLat = this->maxLat;
    maxLon = this->maxLon;
}

const OsmNode&
OsmSection::getNode(long id) const {
    NodeMap::const_iterator entry = nodes.find(id);
    if (entry != nodes.end()) {
        return entry->second;  // found node for given ID
    }
    // No node for given ID (possibly invalid ID)
    return OsmNode::InvalidNode;
}

OsmNode
OsmSection::createNode(const boost::property_tree::ptree& xml) {
    // Extract information from XML attributes for convenience.
    const long   id  = std::stol(xml.get_child("<xmlattr>.id").data());
    const double lat = std::stod(xml.get_child("<xmlattr>.lat").data());
    const double lon = std::stod(xml.get_child("<xmlattr>.lon").data());
    // Create the actual OSM node object
    OsmNode node(id, lat, lon);
    // Add tag child elements (if any)
    for (const auto& child : xml) {
        if (child.first == "tag") {
            node.addTag(child.second.get_child("<xmlattr>.k").data(),
                        child.second.get_child("<xmlattr>.v").data());
        }
    }
    // Return the newly created node
    return node;
}

OsmWay
OsmSection::createWay(const boost::property_tree::ptree& xml) {
    // Extract information from XML attributes for convenience.
    const long   id  = std::stol(xml.get_child("<xmlattr>.id").data());
    // Create the actual OSM way object
    OsmWay way(id);
    // Add tag/node child elements
    for (const auto& child : xml) {
        if (child.first == "tag") {
            way.addTag(child.second.get_child("<xmlattr>.k").data(),
                       child.second.get_child("<xmlattr>.v").data());
        } else if (child.first == "nd") {
            long id = std::stol(child.second.get_child("<xmlattr>.ref").data());
            way.addNode(id);
        }
    }
    // Return the newly created node
    return way;
}

void
OsmSection::load(const std::string& filePath) {
    // Load the XML file into the property tree. If reading fails
    // (cannot open file, parse error), an exception is thrown.
    ptree pt;
    read_xml(filePath, pt);
    // Setup the bounds from the XML data.
    setBounds(pt.get_child("osm.bounds.<xmlattr>"));
    // Process all the child nodes
    for (const auto& child : pt.get_child("osm")) {
        if (child.first == "node") {
            OsmNode node = createNode(child.second);
            nodes[node.getID()] = node;
        } else if (child.first == "way") {
            OsmWay way = createWay(child.second);
            ways.push_back(way);
        }
    }
}

void
OsmSection::printWays(std::ostream& os) const {
    // Print each way with node details.
    for (const OsmWay& way : ways) {
        way.print(nodes, os);
    }
}

#endif
