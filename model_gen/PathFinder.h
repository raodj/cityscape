#ifndef PATH_FINDER_H
#define PATH_FINDER_H

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

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include "IndexedPriorityQueue.h"
#include "PathSegment.h"
#include "OSMData.h"

/** A path consists of a sequence of path segements.  This is a
    shortcut to refer to a vector of path segments.
*/
using Path = std::vector<PathSegment>;

/** A convenience opeator to print an entire path.

    This is a convenience stream-insertion operator to print all the
    segments in a given path.

    \param[in,out] os The output stream to where the path information
    is to be written.

    \param[in] path The path to the printed

    \return This method returns os, as per API convention.
*/
std::ostream& operator<<(std::ostream& os, const Path& path);

class PathFinder {
public:
    /** Constructor create an object with a given OSMData object

        \param[in] osmData The OSM data object that contains the
        building, ways, and nodes data loaded from a generated model
        text file.
    */
    PathFinder(const OSMData& osmData);

    /** Find the best path between a given source and destination
        building.

        \param[in] startBldId The starting ID of the building.  It is
        assumed that the building ID is valid.

        \param[in] endBldId The ending ID of the building.  It is
        assumed that the building ID is valid.

        \param[in] useTime If this flag is true, then distance is
        computed based on time instead of distance.
        
        \param[in] minDist The minimum distance (in miles) around the
        source and destination to explore.

        \param[in] scale The extra miles to expand the minDist based
        on the distance between start and destination segments.
    */
    Path findBestPath(long startBldId, long endBldId, bool useTime = false,
                      const double minDist = -1, const double scale = -1);
    
    /** Find the best path between the source and destination node.

        \param[in] src The source path segment from where the path is
        to be computed.

        \param[in] dest The destination path segement to which the
        path is to be computed.

        \param[in] useTime If this flag is true, then distance is
        computed based on time instead of distance.

        \param[in] minDist The minimum distance (in miles) around the
        source and destination to explore.  If this value is -1 then a
        outer limit ring is not used as an optimization.

        \param[in] scale The extra miles to expand the minDist based
        on the distance between start and destination segments.  If
        this value is -1 then a outer limit ring is not used as an
        optimization.
        
        \return The path fro the source to destination path 
    */
    Path findBestPath(const PathSegment& src, const PathSegment& dest,
                      bool useTime = false,
                      const double minDist = -1, const double scale = -1);

    /** Helper method to generate an xfig file with the path
        information.

        \param[in] path The path to be drawn.

        \param[in] figFilePath The path to the xfig file to where the
        path is to be drawn.

        \param[in] figScale The scale for the output XFIG figure so
        that various shapes and information are readable.
    */
    void generateFig(const Path& path, const std::string& figFilePath,
                     const int figScale = 16384000) const;

    /** Helper method to find the nearest node in a given way that
        contains the given coordinate.

        This method checks pairs of consecutive nodes to see if they
        can contain (but not necessarily on the line segment) the
        specified point.  If so it returns the starting node.

        \param[in] way The way on which the given point is to be
        located.

        \param[in] latitude The latitude of the point to be checked.

        \param[in] longitude The longitude of the point to be checked.

        \return If the way contains the point then this method returns
        a valid index of the first node associated with the segment
        containing the point.  Otherwise this method returns -1.
    */
    int findNearestNode(const Way& way, const double latitude,
                        const double longitude) const;

protected:
    /** Setup a boundary ring that defines the maximum outer limits up
        to which the path finder should explore.  This ring serves an
        optional optimization to ensure that the path finder converges
        quickly.

        \param[in] src The source path segment from where the path is
        to be computed.

        \param[in] dest The destination path segement to which the
        path is to be computed.
 
        \param[in] minDist The minimum distance (in miles) around the
        source and destination to explore.

        \param[in] scale The extra miles to expand the minDist based
        on the distance between start and destination segments.
    */
    void setLimits(const PathSegment& src, const PathSegment& dest,
                   const double minDist = 0.5, double scale = 0.25);
    
    /** Helper method to reconstruct the path from a given destination
        node.

        This method uses the parent segement ID to reconstruct the
        route from the source segement to the destination segment.
        This method uses the information in exploredPaths to rebuild
        the path.

        \param[in] dest The destination node from where the path is to
        be reconstructed.
    */
    Path rebuildPath(const PathSegment& dest) const;

    /** Helper method to find a node in a given way.

        This returns the index position in this way corresponding to
        the given nodeID.

        \param[in] way The way on which the node is located.

        \param[in] nodeID The ID of the node whose index is to be
        returned by this method.

        \return If the way contains the given nodeID, then this method
        returns a valid index of the nodeID.  Otherwise this method
        returns -1.
    */
    int findNode(const Way& way, const long nodeID) const;

    /** Find path between 2 segements on the same way.

        Find the Point either for a give node (if nodeID != -1) or the
        intersection with the way based on a building ID.
        
        \param[in] src The source path segement from where the way is
        to be computed.

        \param[in] dest The destination path segment to where the path
        is to be computed.

        \param[in,out] idCounter A counter that is to be used to
        generate unique-IDs for each path segment generated by this
        method.

        \return A path segement between the two ways.
    */
    Path getPathOnSameWay(const PathSegment& src, const PathSegment& dest);
    
    /** Convenience method to return the latitude and longitude of the
        starting/ending point associated with a given way.

        \param[in] path The path for which the starting/ending
        latitude and longitude is to be returned by this method.

        \return The Point containing the longitude (in Point::first)
        and latitude (in Point::second).
    */
    Point getLatLon(const PathSegment& path) const;

    /** Compute the distance between two path segements.

        This method handles both nodes and building IDs appropriately.
        
        \param[in] ps1 The first path segment to be used to compute
        distance.

        \param[in] ps2 The second path segment to be used to compute
        distance.

        \return The distance between ps1 and ps2 in miles.
     */
    double getDistance(const PathSegment& ps1, const PathSegment& ps2) const;
    
    /** Adds adjacent, unexplored nodes to the heap of exploring path
        segements.

        This method is used to add unexplored nodes ajdacent to a
        given segement.  Typically, seg is the next nearest segement
        to be checked when constructing the best route.  This method
        handles both cases of working with buildings or nodes.

        \param[in] seg The segement (with node or building ID set)
        adjacent to which new entries are to be added to the exploring
        heap.

        \param[in] dest The destination node to which the path is
        being computed.
    */
    void addAdjacentNodes(const PathSegment& seg, const PathSegment& dest,
                          const bool addIntersectingWays = true);

    /** Internal helper method to check and add a given node along a
        given way to the exploring heap.

        This method is used internally by the addAdjacentNodes method.

        \param[in] parent The parent segement from where the path is
        being extended.

        \param[in] nodeID The node ID being added to the path being
        explored.

        \param[in] wayID The ID of the way on which the position of
        the node is being computed.

        \param[in] nodeIndex The index of the node in way's list
        corresponding to the given nodeID.
        
        \param[in] addLoops If the way is tagged to have loops, call
        the checkAddLoopNodes method to add adjacent nodes to
        correctly handle loops.
        
        \return This method returns true if the node was added or
        updated.  Otherwise this method returns false (indicating the
        node has already been explored).
    */
    bool checkAddNode(const PathSegment& parent, const Way& way,
                      const int nodeIndex, const bool addLoops);

    /** Check if a node is repeated in a way that has loops and add
        adjacent nodes.

        \param[in] curr The current path segment that is being added
        to the set of nodes to be explored.

        \param[in] nodeIndex The index of the node whose path is to be
        explored.
    */
    void checkAddLoopNodes(const Way& way, const int nodeIndex,
                           const PathSegment& curr);
    
private:
    /**
       The OSM data object that contains the building, ways, and nodes
       data loaded from a generated model text file.
    */
    const OSMData& osmData;

    /** A counter to generate unique ID for each segement explored by
        this instance.
    */
    long segIdCounter = 0;

    /** A priority queue of path segments that are currently being
        explored to determine best route from source to destination.

        This instance variable maintains the current path being
        explored in priority order -- the priority is determined based
        on distance.  This data structure also permits to look-up
        segements based on their nodeID (via the functor in
        PathSegment).
    */
    IndexedPriorityQueue<PathSegment, long> exploring;

    /** A hash map to ease looking-up path segements that have already
        been explored.  This hash map enables reconstructing the route
        after the destination node has been reached.  The key into
        this hash map is the segement ID associated with each path
        segement.
    */
    std::unordered_map<long, PathSegment> exploredPaths;

    /** A convenience set to quickly look-up nodes that have already
        been explored.  This hash map is used to avoid re-exploring
        nodes that have already been explored (thereby avoiding
        cycles) and removed from the exploring heap.

        \note If nodes are not in this set, then they could be in the
        exploring heap.
    */
    std::unordered_set<long> exploredNodes;

    /** A ring that defines a rectangular boundary around the source
        and destination that is used to limit the scope of nodes
        explored by this path finder.  This is an optimization to
        ensure path finding converges quickly.  This ring is
        initialized to an invalid value.  The ring is setup in the
        setLimits method.
    */
    Ring outerLimits;

    /** A convenience method that is used to set if time should be
        used as a metric instead of distance.  The flag is used in the
        getDistance method in this class to return time instead of
        distance.
    */
    bool distIsTime;
};

#endif
