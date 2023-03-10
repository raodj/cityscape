#ifndef POP_GRID_CPP
#define POP_GRID_CPP

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

#include "PopGrid.h"
#include "Utilities.h"
#include <iostream>

// The process-wide unique singleton instance
PopGrid PopGrid::popGrid;

void
PopGrid::setupGrid(const std::vector<PopRing>& rings) {
    // First determine the top-left and bottom-right coordinates along
    // with width and height of each ring to determine the
    ASSERT(!rings.empty());
    // Ensure we have some valid grid sizes to work with.
    const double width = rings[0].getWidth(), height = rings[0].getHeight();
    ASSERT(width > 0);
    ASSERT(height > 0);
    // Variables to track min & max bounds.  Point::first is longitude
    // and point::second is latitude.
    minTopLeft = Point{rings[0].tlLon, rings[0].tlLat};
    Point maxBotRight{rings[0].brLon, rings[0].brLat};
    // Compute the minimum and maximum coordinates (to eventually
    // determine the overall dimensions of the grid).
    for (const PopRing& pr : rings) {
        minTopLeft.first   = std::min(minTopLeft.first,   pr.tlLon);
        minTopLeft.second  = std::max(minTopLeft.second,  pr.tlLat);
        maxBotRight.first  = std::max(maxBotRight.first,  pr.tlLon);
        maxBotRight.second = std::min(maxBotRight.second, pr.tlLat);
        // Minor differences in width & height can occur due to
        // precision issues in double.
        ASSERT(width  - pr.getWidth()  < 1e-8);
        ASSERT(height - pr.getHeight() < 1e-8);
    }

    // Now we know the extent in latitude and longitude of the
    // rings. It is time to create and populate the grid.
    const int maxRows = (minTopLeft.second - maxBotRight.second) / height + 1;
    const int maxCols = (maxBotRight.first - minTopLeft.first)   / width  + 1;
    ASSERT(maxRows > 0);
    ASSERT(maxCols > 0);
    ringGrid.resize(maxRows, std::vector<PopRing>(maxCols));

    // Now copy the population ring information into appropriate
    // location.
    for (const PopRing& pr : rings) {
        const int row = (minTopLeft.second - pr.tlLat) / height;
        const int col = (pr.tlLon - minTopLeft.first)  / width;
        ASSERT((row >= 0) && (row < maxRows));
        ASSERT((col >= 0) && (col < maxCols));
        ASSERT(!ringGrid[row][col].isValid());
        // Copy the population ring info into the empty slot
        ringGrid[row][col] = pr;
        // Now the entry should be valid.
        ASSERT(ringGrid[row][col].isValid());
        // Record the row, col coordinates for each grid
        ringGridCoords.push_back(GridCoord{row, col});
    }

    // Now build the adjacency search lists using a helper method.
    std::vector<std::vector<bool>> visited(maxRows, std::vector<bool>(maxCols));
    // Start recursive call at the middle of the grid as logical origin 
    buildAdjSearchLists(visited, {maxRows / 2, maxCols / 2}, {0, 0});
    // Print summary information about the grid we have loaded
    std::cout << "Model grid: " << maxRows << "x" << maxCols << " from "
              << rings.size()   << " rings.\n";
}

void
PopGrid::buildAdjSearchLists(std::vector<std::vector<bool>>& visited,
                             const GridCoord& curr, const GridCoord& relative) {
    // Build a fixed set of 9 adjacent neighbors to explore
    const std::vector<GridCoord> Neighbors = {{-1, -1}, {-1, 0}, {-1, +1},
                                              { 0, -1}, {0,  0}, { 0, +1},
                                              {+1, -1}, {+1, 0}, {+1, +1}};
    // Check and explore each neighbor. Other than for origin, explore
    // no more than 3 adjacent neighbors.
    int maxNeighbors = (relative == GridCoord(0, 0)) ? 8 : 3;
    // Set the current node as having been explored.
    visited[curr.first][curr.second] = true;
    // Create list of neighbors to visit.
    std::vector<GridCoord> hood;
    // Add relative coordinates of neighbors in the list
    for (size_t i = 0; ((maxNeighbors > 0) && (i < Neighbors.size())); i++) {
        // Create the row, col for the neighbor.
        const GridCoord neigh{curr.first  + Neighbors[i].first,
                              curr.second + Neighbors[i].second};
        // If the neighbor coordinate is not valid, then don't explore it.
        if ((neigh.first < 0)  || (neigh.first  >= (int) visited.size())    ||
            (neigh.second < 0) || (neigh.second >= (int) visited[0].size()) ||
            visited[neigh.first][neigh.second]) {
            continue;  // This is not a valid or already explored.
        }
        // Create relative offset for this neighbor and add it to the
        // hood.
        visited[neigh.first][neigh.second] = true;
        hood.push_back(Neighbors[i]);
        maxNeighbors--;  // limit neighbor search
    }
    // Add entry for neighborhood, if it is not empty
    if (!hood.empty()) {
        ASSERT(adjSearchLists.find(relative) == adjSearchLists.end());
        adjSearchLists[relative] = hood;
        DEBUG({
                std::cout << relative.first << "," << relative.second << ":";
                for (GridCoord neigh : hood) {
                    std::cout << "  " << neigh.first << "," << neigh.second;
                }
                std::cout << std::endl;
            });
    }
    // Now recursively check the neighborhood nodes.
    for (GridCoord offset : hood) {
        // Create the row, col for the neighbor.
        const GridCoord neigh{curr.first  + offset.first,
                              curr.second + offset.second};
        // Create the logical offset from the origin
        const GridCoord offOrig{relative.first  + offset.first,
                                relative.second + offset.second};
        buildAdjSearchLists(visited, neigh, offOrig);
    }
}

#endif
