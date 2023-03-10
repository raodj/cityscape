#ifndef POP_GRID_H
#define POP_GRID_H

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
#include "PopRing.h"

/** A convenience typedef to work with a matrix of values */
using PopRingGrid = std::vector<std::vector<PopRing>>;

/** Relative or absolute coordinates into the grid.  GridCoord::first
    correspondds to the y-coordinate (i.e., row) and
    GridCoord::second is the x-coordinate (i.e., col)
*/
using GridCoord = std::pair<int, int>;

// Forward declaration for the OSMData class
class OSMData;

/** A class to encapsulate a matrix-like grid of population rings to
    ease taxi location management.

    <p>This class is used to maintain a matrix-like organization of
    population rings that logically manage all the taxis in the
    simulation.  This grid essentially enables quickly locating
    adjacent population rings to hand-off taxis as they logically move
    around.</p>

    \note This class is meant to be used as a singleton
    class. Consequently, use the PopGrid::get() method to get the
    process-wide unique instance of the population grid.
*/
class PopGrid {
    /** Custom hasher for GridCoord */
    struct GridCoordHash {
        std::size_t operator()(const GridCoord& gc) const noexcept {
            // Combine first and second into 1 unique value.
            const long value = (gc.first * 8192L) + gc.second;
            // Return hash of the unique value.
            return std::hash<long>{}(value);
        }
    };

    /** Shortcut reference to an uordered map that provides the list
        of adjacent nodes to search, given the logical offset from a
        root node.  The key into this map is the relative x & y offset
        from a logical root, in the form {0, 0} or {-1, -1}, etc. The
        list provides information about the adjacent neighbors that a
        given agent should search in for information.
    */
    using AdjacentSearchList =
        std::unordered_map<GridCoord, std::vector<GridCoord>, GridCoordHash>;

public:
    /** Obtain a reference to the process-wide unique instance of this
        class.

        \return A reference to the process-wide unique instance of
        this class.
    */
    static PopGrid& get() { return popGrid; }

    /** Setup the matrix of population rings using data in a given model

        This method is typically the first method that is invoked on
        this class to setup the grid of population rings for use
        during simulation.  This method operates as follows:

        <ol>

        <li>First determines the top-left and bottom-right bounds of
        the matrix. </li>

        <li>It uses the lat-lon and width/height of each ring to
        compute the dimensions of the matrix.</li>

        <li>It then copies the data for each entry from the model into
        each grid entry.  It updates the grid-coordinates for each
        ring in the ringGridCoords vector</li>

        <li>Finally, it computes a generic adjacency list to ease
        dispatching events to adjacent grid entries during
        simulation. </li>
        
        </ol>

        \param[in] rings The list of rings from a model.  The rings
        are used to create and populate the grid.
    */
    void setupGrid(const std::vector<PopRing>& rings);

    /** Obtain The minimum top-left coordinate (useful for generating
        xfig output).

        \note The value returned by this method is meaningful only
        after the setupGrid method has successfully completed.

        \return The top-left (or minimum) longitude (in Point::first)
        and latitude (in Point::second).
    */
    Point getMinTopLeft() const { return minTopLeft; }

    /** A vector that maintains the logical position of each ring in
        the matrix of grids.  Entries are added to this vector when
        the setupGrid method is invoked.
    */
    std::vector<GridCoord> ringGridCoords;

protected:
    /** Populate the adjacent nodes to search, given the logical
        offset from a root node.

        This is a recursive method (that calls itself) that explores
        the visited grid to compute the set of adjacent nodes to
        communicate and adds entries to the adjSearchLists map.

        \param[in,out] visited A matrix indicating if a specific
        location has already been visited (and consequently should be
        ignored during recursive calls).

        \param[in] curr The current position within the visited grid.
        The initial recursive call starts from the center of the
        visited grid.

        \param[in] relative The relative offset from the initial
        starting point (i.e., center of grid) that serves as the key
        entry into the adjSearchLists map.
    */
    void buildAdjSearchLists(std::vector<std::vector<bool>>& visited,
                             const GridCoord& curr, const GridCoord& relative);

    
    /** The actual matrix of population ring grids setup from the
        information loaded from a model text file.  This matrix
        actually created and initialized in the setupGrid() method in
        this class.
    */
    PopRingGrid ringGrid;

    /** An uordered map that provides the list of adjacent nodes to
        search, given the logical offset from a root node.  The key
        into this map is the relative x & y offset from a logical
        root, in the form {0, 0} or {-1, -1}, etc. The list provides
        information about the adjacent neighbors that a given agent
        should search in for information.
    */
    AdjacentSearchList adjSearchLists;

    /** The minimum top-left longitude (in Point::first) and latitude
        (in Point::second).  The value in this point is meaningful
        only after the setupGrid method has successfully completed.
    */
    Point minTopLeft;

private:
    /** The default constructor.

        The constructor is protected to ensure that this class is
        never directly instantiated.  Instead, this class is designed
        to be a singleton class.  The process-wide unique instance of
        this class can be obtained via calling the PopGrid::get()
        method in this class.
    */
    PopGrid() {}

    /** No copy constructor to ensure that this singleton object is
        never copied, even by accident.
    */
    PopGrid(const PopGrid&) = delete;
    
    /** The process-wide unique instance of this object.  A reference
        to this object is obtained via call to PopGrid::get()
        method
    */
    static PopGrid popGrid;
};

#endif
