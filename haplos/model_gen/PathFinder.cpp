#ifndef PATH_FINDER_CPP
#define PATH_FINDER_CPP

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

#include <sstream>
#include "PathSegment.h"
#include "PathFinder.h"
#include "Utilities.h"

#ifndef NO_XFIG
// Selectively compile-in support for generating XFig
#include "ShapeFile.h"
#endif

PathFinder::PathFinder(const OSMData& osmData) :
    osmData(osmData), distIsTime(false) {
    // Nothing else to be done for now.
}

Path
PathFinder::findBestPath(long startBldId, long endBldId, bool useTime,
                         const double minDist, const double scale) {
    // Get the building information for quick reference.
    const Building begBld = osmData.buildingMap.at(startBldId);
    const Building endBld = osmData.buildingMap.at(endBldId);    
    // Setup the beginning and destination path segmenets for further
    // processing.
    PathSegment begSeg{begBld.wayID, StartNodeID, begBld.id,    0,
            segIdCounter++, -1};
    PathSegment endSeg{endBld.wayID, EndNodeID,   endBld.id, 1000,
            segIdCounter++, -1};
    // Get the helper method to find the path
    Path path = findBestPath(begSeg, endSeg, useTime, minDist, scale);
    // Return the generated path
    return path;
}

// Find best path between two segements
Path
PathFinder::findBestPath(const PathSegment& src, const PathSegment& dest,
                         bool useTime, const double minDist,
                         const double scale) {
    // Setup a limiting ring if specified by the user.
    if ((minDist != -1) && (getDistance(src, dest) > 0.01)) {
        setLimits(src, dest, minDist, scale);
    }
    // Setup flag to indicat eis distance or time is to be used
    distIsTime = useTime;
    // Add the source and destination to the paths being explored.
    exploring.push(src);
    exploring.push(dest);
    // Keep exploring nodes until we reach the destination segement.
    while (!exploring.empty()) {
        // Explore the next nearest node
        PathSegment next = exploring.top();
        exploring.pop();  // Remove the node being explored.
        // Add node to the set of already explored entries
        ASSERT(exploredPaths.find(next.segID) == exploredPaths.end());
        exploredPaths[next.segID] = next;
        ASSERT(exploredNodes.find(next.nodeID) == exploredNodes.end());
        exploredNodes.insert(next.nodeID);
        // If this path is the destination then we found the best
        // path. Return best path back.
        if (next.nodeID == EndNodeID) {
            if (next.distance < 1000) {
                // Yay! found a good path
                return rebuildPath(next);
            } else {
                // A valid path was not found.
                return {};
            }
        }
        // Add adjacent nodes to the next node to explore
        addAdjacentNodes(next, dest);
    }
    // When control drops here, we have explored nodes and did not
    // find a path.
    return {};
}

// Rebuild the path to destination using information in exploredPaths
Path
PathFinder::rebuildPath(const PathSegment& dest) const {
    Path path;  // The path to be build (in reverse initially)
    path.push_back(dest);
    // Keep adding parent until we get to the source node
    long parentSegID = dest.parentSegID;
    while (parentSegID != InvalidNodeID) {
        // Get and add parent node to the path
        ASSERT(exploredPaths.find(parentSegID) != exploredPaths.end());
        const PathSegment& parent = exploredPaths.at(parentSegID);
        path.push_back(parent);
        // Move onto the parent's parent node (if any)
        parentSegID = parent.parentSegID;
    }
    // Now we need reverse the path to get segements in correct order
    std::reverse(path.begin(), path.end());
    return path;
}

// Check and add a given node to a list of adjacent segments.
bool
PathFinder::checkAddNode(const PathSegment& parent, const Way& way,
                         const int nodeIndex, const bool addLoops) {
    // Check if the node has already been explored.  If so, there is
    // no further opreation to be done.
    ASSERT((nodeIndex >= 0) && (nodeIndex < (int) way.nodeList.size()));
    const long nodeID = way.nodeList[nodeIndex];
    if (exploredNodes.find(nodeID) != exploredNodes.end()) {
        return false;  // node already explored.
    }
    // If the node is not inside our limits ring then ignore node
    ASSERT((nodeID >= 0) && (nodeID < (long) osmData.nodeList.size()));
    const Node& node = osmData.nodeList[nodeID];
    if ((outerLimits.getKind() != Ring::UNKNOWN_RING) &&
        !outerLimits.contains(node.longitude, node.latitude)) {
        return false;  // not out of bounds
    }
    // Create a temporary segement based on parent.
    PathSegment seg{way.id, nodeID, -1, 0, 0, parent.segID};
    const double dist = parent.distance + getDistance(parent, seg);
    // Check to see if the node is currently in the exploring set.  If
    // so, we will need to update it if the new distance is better.
    if (exploring.contains(nodeID)) {
        // Get the current entry for this node.
        seg = exploring.at(nodeID);
        // Check and update if we have a better path
        if (dist < seg.distance) {
            // Yes we have a shorter path. Update path segement.
            seg.update(parent.segID, way.id, dist);
        }
    } else {
        // Add a new path segement entry s this
        seg.segID = segIdCounter++;
        // Update distance information in the new segement
        seg.distance = dist;
    }
    // Add/update the segment information in the heap.
    exploring.push(seg);
    // Check and add adjacent nodes in case this node is repeated on
    // ways with loops.
    if (addLoops && way.hasLoop) {
        checkAddLoopNodes(way, nodeIndex, seg);
    }
    return true;
}

// Handle edge case where a way has loops and nodes could be repeated.
void
PathFinder::checkAddLoopNodes(const Way& way, const int nodeIndex,
                              const PathSegment& curr) {
    ASSERT(way.hasLoop);
    ASSERT((nodeIndex >= 0) && (nodeIndex < (int) way.nodeList.size()));
    // Get the nodeID we are checking.
    const long nodeID = way.nodeList[nodeIndex];
    ASSERT((nodeID >= 0) && (nodeID < (long) osmData.nodeList.size()));
    // Find if the node at given index is repeated. If not nothing much to do
    for (int repeatIndex = nodeIndex + 1;
         (repeatIndex < (int) way.nodeList.size()); repeatIndex++) {
        if (way.nodeList[repeatIndex] == nodeID) {
            // Check and add previous node if relevant and if the street
            // is not a one-way.
            if (!way.isOneWay && (repeatIndex > nodeIndex + 1)) {
                checkAddNode(curr, way, repeatIndex - 1, false);
            }
            // Add next node on the way if one is present.
            if (repeatIndex < (int) way.nodeList.size() - 1) {
                checkAddNode(curr, way, repeatIndex + 1, false);
            }
        }
    }
}

// Find adjacent nodes to be explored near given path segment
void
PathFinder::addAdjacentNodes(const PathSegment& seg, const PathSegment& dest,
                             const bool addIntersectingWays) {
    // We are working with a node.  We should have a way
    // associated with this node so we can find adjacent nodes.
    ASSERT( seg.wayID != -1 );
    ASSERT( osmData.wayMap.find(seg.wayID) != osmData.wayMap.end() );
    const Way& way = osmData.wayMap.at(seg.wayID);
    // There are two cases -- common one is we are working is when we
    // have a valid node ID.
    if (seg.buildingID == -1) {
        // Find the index of this node on this way
        const int nodeIdx = findNode(way, seg.nodeID);
        ASSERT( nodeIdx != -1 );
        // Check and add previous node if relevant and if the street
        // is not a one-way.
        if (!way.isOneWay && (nodeIdx > 0)) {
            checkAddNode(seg, way, nodeIdx - 1, true);
        }
        // Add next node on the way if one is present.
        if (nodeIdx < (int) way.nodeList.size() - 1) {
            checkAddNode(seg, way, nodeIdx + 1, true);
        }
        // Finally, handle intersection nodes where multiple ways
        // meet, if requested (recursive calls don't go into this
        // if-statement.
        if (addIntersectingWays) {
            ASSERT(seg.nodeID < (long) osmData.nodesWaysList.size());
            for (long wayID : osmData.nodesWaysList[seg.nodeID]) {
                if (wayID == seg.wayID) {
                    continue;  // Ignore the way we are already on
                }
                
                // Get the way to check to ensure it is not a dead end
                // so that we don't waste time on exploring dead-ends.
                ASSERT( osmData.wayMap.find(wayID) != osmData.wayMap.end() );
                const Way& way = osmData.wayMap.at(wayID);
                if (way.isDeadEnd && (wayID != dest.wayID)) {
                    // This way is a dead end and the destination is
                    // not on this way. So don't explore it.
                    continue;
                }
                
                // Create a temporary segement on the
                const PathSegment interSeg{wayID, seg.nodeID, -1,
                        seg.distance, seg.segID, seg.segID};
                // Add adjacent nodes on the intersecting way, if
                // applicable (but don't further explore ways)
                addAdjacentNodes(interSeg, dest, false);
            }
        }
    } else {
        ASSERT( seg.buildingID != -1 );
        // Find nearest node to this building and use that as the
        // next node for exploration.
        const Point srcPt  = getLatLon(seg);
        int nearNode       = findNearestNode(way, srcPt.second, srcPt.first);
        ASSERT(nearNode != -1);
        if (way.isOneWay) {
            nearNode++;  // For 1-way we want to start with next node
                         // in the direction of traffic flow.
        }
        const long nodeID  = way.nodeList.at(nearNode);
        ASSERT(nodeID < (long) osmData.nodeList.size());
        ASSERT(exploredNodes.find(nodeID) == exploredNodes.end());
        PathSegment newSeg{seg.wayID, nodeID, -1, 0, segIdCounter++, seg.segID};
        newSeg.distance = seg.distance + getDistance(seg, newSeg);
        exploring.push(newSeg);
        // If this way has loops, then check and add any adjacent
        // nodes, if the nodeID is the repeated one.
        if (way.hasLoop) {
            checkAddLoopNodes(way, nearNode, newSeg);
        }
    }
    // Check and add destination path if it is on this way.
    if (seg.wayID == dest.wayID) {
        ASSERT(dest.buildingID != -1);
        const Point destPt        = getLatLon(dest), srcPt = getLatLon(seg);
        const int nearestSrcNode  = (seg.buildingID == -1) ?
            findNode(way, seg.nodeID) :
            findNearestNode(way, srcPt.second, srcPt.first);
        const int nearestDestNode = findNearestNode(way, destPt.second,
                                                    destPt.first);
        // Decide if the destination is to be updated if the node is
        // adjacent.  The checks can be collapsed into a boolean
        // expression, but left the way they are to improve
        // understanding the logic here.
        bool updateDest = false;
        if (way.isOneWay && (nearestSrcNode == nearestDestNode)) {
            updateDest = true;  // one-way check pass
        } else if (!way.isOneWay &&
                   (std::abs(nearestSrcNode - nearestDestNode) <= 1)) {
            updateDest = true;  // two-way check pass
        }
        // Check and update destination node.
        if (updateDest) {
            // This is the nearest node. Update distance to
            // destination.
            ASSERT(exploring.contains(dest.nodeID));
            PathSegment expDest = exploring.at(dest.nodeID);
            const double dist   = seg.distance + getDistance(seg, expDest);
            if (dist < expDest.distance) {
                // Found a shorter route to destination!
                expDest.update(seg.segID, seg.wayID, dist);
                exploring.update(expDest);
            }
        }
    }
}

// Return lat/lon of node or building's entrance
Point
PathFinder::getLatLon(const PathSegment& path) const {
    // Return value from the node as the reference.
    if (path.buildingID == -1) {
        const Node& node = osmData.nodeList.at(path.nodeID);
        return Point(node.longitude, node.latitude);
    }
    // NodeID is -1. So we are working with a building in this situtation.
    ASSERT( path.buildingID != -1 );
    const Building& building = osmData.buildingMap.at(path.buildingID);
    return Point(building.wayLon, building.wayLat);
}

// Simplification to find path where src and dest are on the same way
Path
PathFinder::getPathOnSameWay(const PathSegment& src, const PathSegment& dest) {
    // This method is designed to work with 2 segements on the same
    // way.  So ensure that pre-condition is met.
    ASSERT (src.wayID == dest.wayID);    
    // Get the way and points (longitude, latitude) used below.
    const Way& way = osmData.wayMap.at(src.wayID);
    // Get the points associated with the starting and ending.
    const Point srcPt = getLatLon(src), destPt = getLatLon(dest);
    // Find the nearest nodes on the way for the source and
    // destination to determine the path between the two (assuming,
    // the path is even possible due to the directionality of the
    // way).
    const int nearestNode1 = (src.nodeID != -1) ? findNode(way, src.nodeID) :
        findNearestNode(way, srcPt.second, srcPt.first);
    const long nearestNode2 = (dest.nodeID != -1) ? findNode(way, dest.nodeID) :
        findNearestNode(way, destPt.second, destPt.first);
    ASSERT( nearestNode1 != -1 );
    ASSERT( nearestNode2 != -1 );
    // Handle the edge case in a one-directional way case first.
    if (way.isOneWay) {
        // Here we check the case of one-directional ways to see if
        // the path is even viable.  There are three possible cases:
        if (nearestNode1 > nearestNode2) {
            // Nodes are in opposite directions on a 1-way road.  So
            // a path is not really possible.
            return {};
        } else if (nearestNode1 == nearestNode2) {
            // The nearest nodes on the way are the same. So we need a
            // finer distance metric to be able to decide.
            const Node& near    = osmData.nodeList.at(nearestNode1);
            const int dist2src  = ::getDistance(near.latitude, near.longitude,
                                                srcPt.second, srcPt.first) /
                (distIsTime ? way.maxSpeed : 1);
            const int dist2dest = ::getDistance(near.latitude, near.longitude,
                                                destPt.second, destPt.first) /
                (distIsTime ? way.maxSpeed : 1);
            if (dist2src > dist2dest) {
                // The nodes are in opposite directions on a 1-way
                // street. So no direct path possibe
                return {};
            }
        }
    }
    // A path from src to dest is possible. So build the path
    Path path   = {src};
    const int startIdx = (src.nodeID  == -1) ? nearestNode1 + 1 : nearestNode1;
    const int endIdx   = (dest.nodeID == -1) ? nearestNode2 + 1 : nearestNode2;
    for (int idx = startIdx; (idx < endIdx); idx++) {
        const long segID  = segIdCounter++;
        const long nodeID = way.nodeList[idx];        
        PathSegment seg{way.id, nodeID, -1, 0, segID, path.back().segID};
        seg.distance = getDistance(path.back(), seg);
        path.push_back(seg);
    }
    PathSegment seg{dest.wayID, dest.nodeID, dest.buildingID, 0,
            segIdCounter++, path.back().segID};
    seg.distance    = getDistance(path.back(), seg);
    path.push_back(seg);
    // Return the path segement with distance setup
    return path;
}

void
PathFinder::generateFig(const Path& path, const std::string& xfigFilePath,
                        const int figScale) const {
#ifndef NO_XFIG    
    // Print some stats about the path
    std::cout << "Exploring: " << exploring.size() << ", explored paths: "
              << exploredPaths.size() << ", explored nodes: "
              << exploredNodes.size() << std::endl;
    // The file to which the xfig file is to be written
    if (xfigFilePath.empty()) {
        return;  // Nothing else to be done.
    }
    // The list of vertex coordinates to be drawn
    std::vector<double> xCoords, yCoords;
    std::vector<Ring::Info> vertInfo;
    // Add each path segment to the shape file
    for (size_t i = 0; (i < path.size()); i++) {
        // Get the segement to be processed
        const PathSegment& seg = path[i];
        // Get the lat, lon for the path segement
        const Point lonLat = getLatLon(seg);
        xCoords.push_back(lonLat.first);   // Make list of x and y
        yCoords.push_back(lonLat.second);  // coordinates
        // Create an informational string for the node -- currrently
        // it causes some issue when the fig file is viewed. So it is
        // commented out.
        /* std::ostringstream os;
           os << seg;
           vertInfo.push_back(Ring::Info{0, std::to_string(i), os.str()});
        */
    }
    // Create a ring with the coordinates and the information we have
    // built
    Ring route(0, 0, Ring::ARC_RING, xCoords.size(), &xCoords[0],
               &yCoords[0], vertInfo);
    // Add route to a shape file to make drawing easier
    ShapeFile shpFile;
    shpFile.addRing(route);
    shpFile.genXFig(xfigFilePath, figScale, false, {});
#else
    throw std::runtime_error("generateFig is not complied-in");
#endif
}

// Return index of node with the same ID
int
PathFinder::findNode(const Way& way, const long nodeID) const {
    // We want to iterate only to last-but-one node due to logic in
    // the for-loop below.
    const int NumPoints = way.nodeList.size();
    // Iterate over pairs of nodes and return index of node.
    for (int i = 0; (i < NumPoints); i++) {
        if (way.nodeList[i] == nodeID) {
            return i;  // found the node
        }
    }
    // Node not found on this way.
    return -1;
}

// Find the node that is closest to a gien lat/lon
int
PathFinder::findNearestNode(const Way& way, const double latitude,
                            const double longitude) const {
    // We want to iterate only to last-but-one node due to logic in
    // the for-loop below.
    ASSERT(way.nodeList.size() > 1);
    const int NumPoints = way.nodeList.size() - 1;
    // Iterate over pairs of nodes and return index of node.
    for (int i = 0; (i < NumPoints); i++) {
        // Reference to 2 nodes for comparison.
        const Node& n1 = osmData.nodeList.at(way.nodeList[i]);
        const Node& n2 = osmData.nodeList.at(way.nodeList[i + 1]);
        // Check if given point lies between these nodes.
        if (inBetween(n1.latitude,  n2.latitude,  latitude)  &&
            inBetween(n1.longitude, n2.longitude, longitude)) {
            return i;
        }
    }
    // No valid pair of nodes were found
    return -1;
}

// Return distance (in miles) between 2 path segements.
double
PathFinder::getDistance(const PathSegment& ps1, const PathSegment& ps2) const {
    // Get the latitude and longitude associated with the path segements
    const Point pt1    = getLatLon(ps1);
    const Point pt2    = getLatLon(ps2);
    const double speed = (distIsTime ? osmData.wayMap.at(ps2.wayID).maxSpeed
                          : 1);
    ASSERT(speed > 0);
    // Return the distance between the two
    return ::getDistance(pt1.second, pt1.first, pt2.second, pt2.first) / speed;
}

// Print a path -- an array of PathSegement objects
std::ostream& operator<<(std::ostream& os, const Path& path) {
    if (path.empty()) {
        os << "Empty path\n";
    } else {
        for (const PathSegment ps : path) {
            os << ps << std::endl;
        }
    }
    return os;
}

void
PathFinder::setLimits(const PathSegment& src, const PathSegment& dest,
                      const double minDist, double scale) {
    // Get the source and destination points
    const Point srcPt   = getLatLon(src);
    const Point destPt  = getLatLon(dest);
    // Get the distance (in miles) from src to dest
    const double dist   = ::getDistance(srcPt.second,  srcPt.first,
                                        destPt.second, destPt.first);
    // Compute the buffer distance
    const double maxDist = minDist + (dist * scale);
    // Have the ring build limits around the src & dest
    outerLimits = Ring::createRectRing(srcPt, destPt, maxDist);
}

#endif
