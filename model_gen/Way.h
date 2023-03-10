#ifndef WAY_H
#define WAY_H

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
#include <string>
#include <iostream>

/** A simple class to encapsulate series of nodes constituting a way
    representing a street/road/freeway etc.  The information extracted
    from a parsed Open Street Map XML file.  This class essentially
    encapsulates the following information:

    \code
        <node id="252643632" lat="41.9538873" lon="-87.654928" version="1">
            <tag k="entrance" v="yes"/>
        </node>
    \endcode
   
    Each way is identified by a unique id number.  This number is used
    for cross referencing.
*/
class Way {
public:
    /** The different types of way's that are included in the model */
    enum Kind:int {service, residential, primary, secondary, tertiary,
            motorway, motorway_link, trunk, primary_link, trunk_link,
            secondary_link, tertiary_link, unknown_way};
    
    /** The unique ID associated with this Way */
    long id = -1;

    /** The list of nodes consituting this way */
    std::vector<long> nodeList;

    /** The type of this ay */
    Way::Kind kind = Way::unknown_way;

    /** The maximum speed associated with this way */
    int maxSpeed = -1;

    /** Flag to indicate if the way is a dead-end or culdesac (i.e.,
        no nodes with reference count > 2) */
    bool isDeadEnd = false;

    /** Flag to indicate if the traffic on this road flows in only 1
        direction.  If this flag is true, then the traffic flows in
        only one direction -- the direction in which the nodeList has
        been specified.
    */
    bool isOneWay = false;

    /** Flag to indice if this way has a loop in it -- that is one or
        more nodes are repeated.  This flag is useful to handle this
        cyclic edge case when computing routes.
    */
    bool hasLoop = false;

    /** The number of buildings with entrances on this way */
    int numBuildings = 0;

    /** The name for the way extracted from the OSM file */
    std::string name;

    /**
     * Returns the kind of this way as a string for printing
     *
     * \return The kind of this way as a string representation for
     * logging/printing.
     */
    std::string getKindStr() const;
    
    /** Write this Way to a given output stream.

        This is a helper method that is used to write the information
        associated with this way to a given output stream.

        \param[out] os The output stream to where the information
        about this way is to be written.

        \param[in] writeHeader If this flag is true then a simple
        comment with the order of the fields is written for future
        reference.
        
        \param[in] delim An optional delimiter between each value
        associated with this node.
    */
    void write(std::ostream& os, const bool writeHeader = false,
               const std::string& delim = " ") const;
    
    /** Load the data for this way from a given input stream.

        This is a helper method that is used to read information for
        this way from a given input stream.  This method assumes that
        the data has been written by a prior call to the write method
        in this class.

        \param[in,out] is The input stream from where the information
        for this way is to be read.
    */
    void read(std::istream& is);
};

#endif
