#ifndef OSM_DATA_H
#define OSM_DATA_H

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

#include <unordered_map>
#include <vector>

#include "Node.h"
#include "Way.h"
#include "Building.h"

/** A simple class to encapsulate the model-data generated from OSM
    XML.  This class provides the core data that is used by other
    classes like PathFinder (to find path), etc.
*/
class OSMData {
public:
    /** Internal helper method to load nodes, ways, and buildings from
        a given model file.  Specifically it loads the data into
        nodeList, wayMap, and buildingMap instance variables in this
        class.

        \param[in] modelFilePath The path to the text file from where
        model data is to be loaded. This text/model file must have
        been generated by ModelGenerator in order to correctly
        operate with this method.
     */
    int loadModel(const std::string& modelFilePath);
    
    /** The list of nodes loaded from the model file.  The nodes are
        stored as a vector as they are generated as a contiguous list
        by ModelGenerator.  This eases rapid look-up of nodes when
        finding path
    */
    std::vector<Node> nodeList;

    /** The unordered map that holds information about the ways in a
        given model.  This hash map is populated in the extractWays
        method.  The hash map enables rapid look-up of road
        information, given the unique ID of a way.  The look-up is
        used when routing traffic.
    */    
    std::unordered_map<long, Way> wayMap;

    /** The unordered map that holds information about buildings in a
        given model file.  The hash map enables rapid look-up of
        building information, given the unique ID of a building.
    */    
    std::unordered_map<long, Building> buildingMap;

    /** The list of ways associated with a given node. Nodes that are
        intersections will have one or more ways associated with them.
        This vector provides a convenient mapping to look-up ways
        intersecting at a given node when computing path.
    */
    std::vector<std::vector<long>> nodesWaysList;    

protected:
    /** Add Way entries nodesWaysList vector in this class.  This
        information is handy to quickly find the list of ways
        intersecting at a given node.  This method is called right
        after the model has been loaded in the loadModel() class.
    */
    void computeNodesWaysList();
    
};

#endif