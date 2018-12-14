#ifndef NODE_H
#define NODE_H

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

#include <string>
#include <iostream>

/** A simple class to encapsulate node information extracted from
    parse XML file.  This class essentially encapsulates the following
    information:

    \code
        <node id="252643632" lat="41.9538873" lon="-87.654928" version="1">
        ...
        </node>
    \endcode
   
    Each node is identified by a unique id number.  This number is
    used for finding the node quicky when processing buildings or
    street information.
*/
class Node {
public:
    /** The unique id set for this node.  This is the same id value
        that was in the OSM XML file */
    long osmId = 0;

    /** The latitude value associated for this node */
    double latitude = 0;

    /** The longitude value associated for this node */
    double longitude = 0;

    /** Flag to indicate if this node is an entrance to a building */
    bool isEntrance = false;

    /** The number of times this node was referenced either as a
        building or a street.  This counter is used to finally
        filter-out nodes that are not really used.
    */
    int refCount = 0;

    /** Write this node to a given output stream.

        This is a helper method that is used to write the ifnormation
        associated with this node to a given output stream.

        \param[out] os The output stream to where the information
        about this node is to be written.

        \param[in] writeHeader If this flag is true then a simple
        comment with the order of the fields is written for future
        reference.
        
        \param[in] delim An optional delimiter between each value
        associated with this node.
    */
    void write(std::ostream& os, const bool writeHeader = false,
               const std::string& delim = " ") const;

    /** Load the data for this node from a given input stream.

        This is a helper method that is used to read information for
        this node from a given input stream.  This method assumes that
        the data has been written by a prior call to the write method
        in this class.

        \param[in,out] is The input stream from where the information
        for this node is to be read.
    */
    void read(std::istream& is);
};

#endif
