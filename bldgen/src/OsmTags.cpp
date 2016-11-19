#ifndef OSM_TAGS_CPP
#define OSM_TAGS_CPP

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

#include "OsmTags.h"

// The constant for returning empty string
const std::string OsmTags::EmptyString;

// The constructor that also acts as the default constructor
OsmTags::OsmTags() {
    // Nothing else to be done in the constructor.
}

OsmTags::~OsmTags() {
    // Nothing to be done in the destructor.
}

int
OsmTags::findTag(const std::string& key) const {
    for (size_t i = 0; (i < tags.size()); i++) {
        if (tags[i].first == key) {
            return i;
        }
    }
    return -1;  // key not found
}

void
OsmTags::addTag(const std::string& key, const std::string& value) {
    const int idx = findTag(key);  // see if key already exists
    if (idx != -1) {
        // Key already exists.  Just update its value.
        tags[idx].second = value;
    } else {
        // Key not found. Add a new entry to the list of tags.
        tags.push_back(TagKeyVal(key, value));
    }
}

const std::string&
OsmTags::operator[](const size_t index) const {
    if (index < tags.size()) {
        return tags[index].second;  // valid index.
    }
    return EmptyString;  // invalid index
}

const std::string&
OsmTags::operator[](const std::string& key) const {
    return operator[](findTag(key));
}

std::ostream& operator<<(std::ostream& os, const OsmTags& node) {
    os << "tags={";
    std::string delim;
    for (const auto& tag : node.tags) {
        os << delim << tag.first << "=" << tag.second;
        delim = ", ";
    }
    os << "}";
    return os;
}

#endif
