#ifndef NODE_CPP
#define NODE_CPP

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

#include "Node.h"

void
Node::write(std::ostream& os, const bool writeHeader,
            const std::string& delim) const {
    // Write optional header if requested
    if (writeHeader == true) {
        os << "# Node"     << delim << "osmID "    << delim
           << "latitude"   << delim << "longitude" << delim
           << "isEntrance" << delim << "refCount\n";
    }
    // Write the information for this node.
    os << "node"    << delim << osmId      << delim << latitude << delim
       << longitude << delim << isEntrance << delim << refCount << std::endl;
}

void
Node::read(std::istream& is) {
    std::string node;
    is >> node;
    // Read the information for this node.
    is >> osmId >> latitude >> longitude >> isEntrance >> refCount;
}

#endif
