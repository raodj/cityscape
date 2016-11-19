#ifndef CONFIGURATION_H
#define CONFIGURATION_H

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

/** Class to encapsulate various command-line arguments used by the
    building generator.

    <p>This class is designed to isolate the command-line arguments in
    a meanigful way.  Currently, most of these parameters are used by
    the BuildingGenerator class.  Consequently, that class has been
    made a friend to enable access to the configuration
    information.</p>

    <p>This class is also responsible for parsing the command-line
    arguments and populating the various instance variables with the
    values specified by the user.</p>
*/
class Configuration {
    friend class BuildingGenerator;

public:
    /** The default constructor.  Merely initializes the various
        instance variables to default initial values.
    */
    Configuration();

    /** The destructor.

        This is just a place holder as the destructor does not have
        any special tasks to perform (because this class does not
        directly use any dynamic memory).        
    */
    ~Configuration();

    /** Convenience method to parse command-line arguments.

        This method uses a argument parser (borrowed from MUSE
        project) to parse command-line arguments into various instance
        variables in this class.

        \param[in] argc The number of entries in the argv[] array.

        \param[in] argv The actual set of command-line arguments.

        \return This method returns true if the command-line arguments
        were successfully processed.  On errors this method returns
        false.
    */
    bool process(int& argc, char *argv[]);

protected:
    
private:
    /** The path to the input OSM XML file (if any) specified by the
        user. This is a command-line option that is supplied by the
        user via the `--osm-xml` option.  This option should always be
        specified with a valid OSM XML file.
    */
    std::string osmXmlPath;

    /** Flag to indicate if the ways loaded from the OSM XML file
        should be printed.  The output is ways (with node information
        resolved) from the OsmSection class.  The output data is
        useful for troubleshooting purposes.  This option can be
        enabled with the `--print-ways` command-line argument.
    */
    bool printWays;
    
    /** The path to the output XFIG file to which a map with ways and
        buildings is to be generated.  This is optional.  If a file
        name is not specified then fig output is not generated.  This
        value is set via the `--fig-file` command-line argument.
    */
    std::string xfigPath;

    /** The zoom level at which the figure is to be drawn. The default
        zoom level is 15 to provide sufficient detail for small
        sections.  This value is set via the `--zoom-level` flag.
        This value is meaningful only if the `--fig-file` argument is
        also specified.
    */
    int zoomLevel;
    
    /** Flag to indicate if road ways from the input OSM XML file must
        be drawn in the output fig file.  This is very useful to
        provide detailed information for verification and
        troubleshooting.  This option can be enabled via `--draw-ways`
        command-line argument.
    */
    bool drawWays;

    /** Flag to indicate if buildings generated are to be drawn in the
        output fig file.  The default value is true.  This option can
        be enabled via `--draw-buildings` option.
    */
    bool drawBuildings;

    /** The directory where the images associated with the output fig
        file are to be cached.  If image tiles are already present in
        the cache directory then they are not downloaded.  This makes
        the process of generating fig output significantly faster.
        Use the `--cache-dir` to set this value.  The default value
        for this option is "../cache"
    */
    std::string cacheDir;
};

#endif
