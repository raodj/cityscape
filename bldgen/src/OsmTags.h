#ifndef OSM_TAGS_H
#define OSM_TAGS_H

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

#include <string>
#include <utility>
#include <vector>
#include <iostream>

/** Nodes and Ways in OSM XML can have additional attributes specified
    via &lt;tag/&gt; XML elements.  Each tag as a key and value
    attribute.  Here TagKeyVal::first holds the key while
    TagKeyVal::second holds the value associated with the key.
*/
using TagKeyVal = std::pair<std::string, std::string>;

/** A list of tags associated with a given OSM node/way. Tags are
    added and searched based on the key value associated with each
    tag.
*/
using TagList = std::vector<TagKeyVal>;

/** A simple class to encapsulate information about tags associated
    with a OSM node/way.  This class provides a reusable approach for
    managing &lt;tag/&gt; elements in OSM XML.  The tags are stored as
    a vector (ideally an unordered_map would be better but it is not
    used) to ensure a compact memory footprint to handle large OSM
    sections.
*/
class OsmTags {
    /** Convenience method to print all the tags as a comma separated
        list of values.

        \param[out] os The output stream to where the data is to be
        written.

        \param[in] tags The OsmTags node whose values are to be
        printed.

        \return The output stream passed in as the parameter.        
    */
    friend std::ostream& operator<<(std::ostream& os, const OsmTags& tags);

public:
    /** Convenience (and default) constructor to create empty Tags.

        The default constructor is required to ease using this class
        with other STL containers (if the need arises).
    */
    OsmTags();

    /** The destructor.

        This is just a place holder as the destructor does not have
        any special tasks to perform (because this class does not
        directly use any dynamic memory).
     */
    ~OsmTags();

    /** Add a new tag to the list of tags associated with this OSM
        node entry.

        \param[in] key The key to be added to the list of tags.  If
        this key already exists, then its value is replaced by this
        method.  Otherwise a new tag entry is added.

        \param[in] value The value associated with this the key.
    */
    void addTag(const std::string& key, const std::string& value);

    /** Convenience method to find a given key in the list of tags
        associated with this node.

        \param[in] key The key to be found.

        \return If the key is found then this method returns a valid
        index.  Otherwise this method returns -1.
    */
    int findTag(const std::string& key) const;

    /** Obtain the tag value associated with a given index.

        \param[in] index The index position in the tags list for which
        the value is to be returned.  The index is typically obtained
        via call to the findTag() method in this class.

        \return The value associated with this given index.  If the
        index is not valid then this method returns an empty string.
    */
    const std::string& operator[](const size_t index) const;

    /** Obtain the tag value associated for the given key.

        \param[in] key The key in the tags for which the value is to
        be returned.

        \return The value associated with this given key if found.
        Otherwise this method returns an empty string.
     */
    const std::string& operator[](const std::string& key) const;

    /** A constant empty string to be returned by methods in this
        class
    */
    static const std::string EmptyString;
    
private:
    /** The list of tags associated with this node. Each tag consists
        of key-value pair which is stored using the TagKeyVal entries.
        Entries are added via the addTag method.  Tags can be
        retrieved using the overloaded operator[] or the find method.
    */
    TagList tags;
};

#endif
