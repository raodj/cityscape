#ifndef MODEL_GENERATOR_H
#define MODEL_GENERATOR_H

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
#include <set>
#include "Node.h"
#include "Way.h"
#include "ShapeFile.h"
#include "PopulationRingGenerator.h"
#include "rapidxml.hpp"
#include "ArgParser.h"
#include "Building.h"
#include "PUMS.h"

/** A shortcut to a pair that holds the key and value */
using KeyValue = std::pair<std::string, std::string>;

class ModelGenerator {
public:
    /** The top-level method that coordinates the various tasks.
        
        This method essentially calls various internal helper methods
        in the following order:

        <ol>

        <li>First it calls loadShapeFile method which loads the
        community shape file and metadata from the shape file
        (specified via \c --shape) and DBF file (specified via \c
        --dbf) into the shpFile object.</li>

        <li>Next it calls createPopulationRings helper method.  This
        method loads population data from the GIS file specified via
        --pop-gis command-line argument.  It then uses
        PopulationRingGenerator class to generate rectangular
        population grids corresponding the community shapes loaded
        from the shape files in the previous step.  The population
        rings are stored in the popRings instance variable in this
        class.</li>

        <li>In the third step, it loads the building data from the
        specified osm-xml file.  It creates a list of building objects
        using the data from the OSM XML file. Each building is
        assigned to corresponding population rings to ease generation
        of human populations.</li>

        <li>Finally, the population data is used to assign people
        living each bulding.</li>
        
        </ol>
    */
    int run(int argc, char *argv[]);
    
protected:
    /** Internal helper method to parse command-line arguments and
        poulate the internal cmdLineArgs object with pertinent
        information.

        This method internally uses the ArgParser to parse out the
        command-line argument specified by the user.

        \param[in] args The number of command-line arguments -- i.e.,
        the size of the argv array.

        \param[in] argv The actual command-line arguments.

        \return This method returns zero if the command-line arguments
        were successfully processed.  On errors it returns a non-zero
        error code.
    */
    int processArgs(int argc, char *argv[]);

    /** Load the shape file and DBF file from the specified input
        files.

        This method is typically the first method that is invoked to
        load the community shape files and associated metadata from
        the the given command-line arguments.

        \return This method returns zero if the command-line arguments
        were successfully processed.  On errors it returns a non-zero
        error code.        
    */
    int loadShapeFile();

    /** Load the PUMS PUMA CSV, SHP, and DBF files from the input
        files specified by the user as command-line arguments.  The
        data is actually loaded by the PUMA class.  This is a helper
        method in this class that calls the method in the PUMA class.

        This method is called before the building generation process
        commences.  The community-shape data and its bounds are used
        to constrain the region of PUMA/PUMS data loaded.

        \return This method returns zero if the command-line arguments
        were successfully processed.  On errors it returns a non-zero
        error code.        
    */
    int loadPUMA();
    
    /** Internal method to load population from GIS data and create
        rectangular population rings.

        This method internally uses the PopulationRingGenerator to
        create the population rings corresponding to the shapes loaded
        by the loadShapeFile method.
    */
    void createPopulationRings();

    /** Write pertinent community and population rings in XFIG format
        to a given file -- assuming an output fig file was specified
        via the \c --xfig command-line argument.
    */
    void generateFig();

    /** Helper method to load and parse the specified OSM XML file.
        This method loads the XML file and processes the XML using
        RapidXML into osmXML.
    */
    void loadOsmXml();

    /** Extract node IDs with latitude and longitude values from the
        parsed XML data. The nodes are stored in an hash-map to enable
        rapid look-up when processing building and roadway entries.
    */
    void extractNodes();

    /** Extract ways (streets, roads, highways, etc.) from the parsed
        XML data. The nodes are stored in a hash-map to enable rapid
        look-up when processing building and roadway entries.

        \note This method must be called only after extractNodes
        method has been processed.
    */
    void extractWays();
    
    /** Helper method to process the OSM XML data to create buildings,
        corresponding to houses or apartments where people live.
    */
    void createBuildings();

    /** Convenience method to extract the key and value attribute for
        a tag element in OSM XML */
    KeyValue getKeyValue(const rapidxml::xml_node<>* node,
                         const std::string& key,
                         const std::string& value) const;

    /* Get the population ring associated with a given building.

       \param[in] building The ring corresponding to the polygon for
       the building.  The centroid of the building is used to
       determine if the building lies within a population ring.
       
       \return The index of the ring in popRings vector that contains
       the centroid for the building.  Otherwise this method returns
       -1.
    */
    int getPopRing(const Ring& building) const;

    /* Get the population ring associated with a given latitude and
       longitude.

       \param[in] latitude The latitude of the point

       \param[in] longitude The longitude of the point
       
       \return The index of the ring in popRings vector that contains
       the centroid for the building.  Otherwise this method returns
       -1.
    */
    int getPopRing(const double latitude, const double longitude) const;
    
    /** Helper method to get number of buildings and total square
        footage of the homes for a given population ring.

        \param[in] popRingID The ID of the population ring for which
        the number of buildings is to be computed.

        \param[out] bldCount The count of number of buildings found
        for the given population ring.
        
        \param[out] bldSqFt The sum of the square footage of all the
        buildings (number of levels is taken into account) for the
        given population ring.
        
        \param[out] homeCount The count of number of buildings that
        are homes.

        \param[out] homeSqFt The sum of the square footage of the
        homes (number of levels is taken into account) for the given
        population ring.
    */
    void getTotalSqFootage(const int popRingID, int& bldCount, long& bldSqFt,
                           int& homeCount, long& homeSqFt) const;

    /** Internal helper method to adjustments to generated model
        (specified via --adjust-model command-line argument).
        
        Ajustments include loading IDs of buildings to be ignored and
        remapping of poulation from one ring to another.

        Some of the buildings in the data are to be ignored when
        generating buildings to distribute human population.  These
        building IDs are to be ignored because they are not tagged
        correctly in OSM and they significantly throw off area
        calculations.  This method poulates the set bldIgnoreIDs based
        on the data from the text files.

        This method also populates the popRemap hash map with index of
        population rings whose population is to be remapped to a given
        destination ring.  This is done to account for minor
        differences between open street map and LandScan data.
    */
    void loadModelAdjustments();

    /** Convenience method to cross-reference a way to a population
        area.  This method essentially adds entries to the
        popAreaWayID vector in this class. Note that this method
        assumes that the popAreaWayIDs vector has been resized to
        match the population areas.

        \note This method cannot correctly detect very long (say over
        0.57 mile straint line stretch) roads cutting across a
        population ring without any intermediate nodes inside a
        population ring.
    
        \param[in] way The way whose points are to be used to
        determine the population areas they intersect with.

        \return This method returns true if at least one population
        ring was found for this way, indicating this way is involved
        within the regions of this model.
     */
    bool addWayToPopRings(const Way& way);

    /** Internal helper method to check if a given XML node is a
        building entry and return a building object, if it is.

        \note This method is called from multiple threads.
        
        This method is an internal helper method that is called from
        createBuildings method (from many threads).  This method
        checks to see if a given XML node is valid building node. A
        valid building node is determined based on several checks
        performed by this method and its sub-helper method
        processBuildingElements.  If the node is a valid building
        node, then this method creates a suitable building object and
        returns it.

        \param[in] node The node to be checked.  This node cannot be
        nullptr.

        \param[out] vertexLat This is just a reused vector to store
        latiatude / longitude values temporarily.

        \param[out] vertexLat This is just a reused vector to store
        latiatude / longitude values temporarily.

        \param[in,out] pumaIndex The index into the puma rings from
        where checks for the PUMA area is to commence.  This value is
        essentially the index of the PUMA ring assigned to a building
        in the previous check (initially it is 0).  This is done to
        use spatial-locality of generating buildings to reduce PUMA ID
        assignment. 
        
        \return A suitable building object if node is valid.
        Otherwise it returns an invalid building object with the
        building ID set to zero.
     */
    Building checkExtractBuilding(rapidxml::xml_node<>* node,
                                  std::vector<double>& vertexLat,
                                  std::vector<double>& vertexLon,
                                  int& pumaIndex) const;

    /** Internal helper method to check if sub-lements of a given XML
        node corresponds a building entry and extract information.

        This method is an internal helper method that is called from
        checkExtractBuilding method (from many threads).  This method
        checks to see if child-nodes have valid building
        information.  A valid building node is determined based on
        several checks performed by this method and its sub-helper
        method processBuildingElements.  If the node is a valid
        building node, then this method return true.

        \param[in] node The node to be checked.  This node cannot be
        nullptr.

        \param[out] vertexLat This is just a reused vector to store
        latiatude / longitude values temporarily.

        \param[out] vertexLat This is just a reused vector to store
        latiatude / longitude values temporarily.

        \param[out] type The type of building extracted from XML
        sub-elements.

        \param[out] levels The number of levels in this building if
        included in an XML sub-element.

        \param[out] nodes The list of node IDs processed for this
        building.  The node IDs are used further to determine the
        nearest street intersection for this building.

        \param[out] isHome Flag that is set to true if the building's
        elements suggest that it is a residential building.
        
        \return This method returns true if the sub-elements indicate
        that this is a valid building node.
    */
    bool processBuildingElements(rapidxml::xml_node<>* node,
                                 std::vector<double>& vertexLat,
                                 std::vector<double>& vertexLon,
                                 std::string& type, int& levels,
                                 std::vector<long>& nodes,
                                 bool& isHome) const;

    /** Helper method to generate an arc-ring for a given way to ease
        plotting


        \param[in] way The way whose nodes are to be convereted to a Ring

        \return A ring containing teh nodes of the Way.
    */
    Ring getRing(const Way& way) const;

    /** Convenience method to determine the street and intersection on
        the street for pick-up/drop-off location for a given building.
        This method operates as follows:

        <ol>

        <li>It obtains the entrance to the building by calling
        findEntrance() helper method in this class.</li>
        
        <li>Next, it iterates over each way associated with the given
        population ring and computes the nearest perpendicular
        distance to each segment in the way by calling the
        getShortestDistance()</li>

        <li>The shortest distance and point is tracked and
        returned.</li>
        
        </ol>

        \param[in] ring The ring associated with the building whose
        entrance is to be determined.

        \param[in] popRingID The population ring for this building.
        This is used to narrow down the ways that could be near
        building.

        \param[in] nodeList The list of node IDs associated with each
        vertex of the building.  This list is used to determine if any
        of the vertices have been tagged as the preferred entrance to
        the building.
        
        \param[out] wayLat Latitude for the intersection on the
        nearest way that will serve as pick-up/drop-off point.

        \param[out] wayLon Longitude for the intersection on the
        nearest way that will serve as pick-up/drop-off point.
        
        \return The ID of the closest way for this building.
    */
    long findNearestIntersection(const Ring& bldRing, const int popRingID,
                                 const std::vector<long>& nodeList,
                                 double& wayLat, double& wayLon) const;

    /** This overloaded method finds the minimum perpendicular
        distance between a given entrance point to segments with the
        given way.

        \param[in] entrance The entrance to a given buliding.
        
        \param[in] way The way to which minimum perpendicular distance
        from the entrance is to be computed.

        \param[out] wayLat The latitude (on the way) for minimum
        perpendicular distance.

        \param[out] wayLon The longitude (on the way) for minimum
        perpendicular distance.

        \return The minimum perpendicular distance (in miles) between
        the entrance and the way.
    */
    double findNearestIntersection(const Point& entrance, const Way& way, 
                                   double& wayLat, double& wayLon) const;

    /** Helper method to find perpendicular intersection between a
        given entrance and a segment of a way.

        \param[in] entrance The entrance point of a given building.

        \param[in] node1 The first node for the given way's segment.

        \param[in] node2 The second node for the given way's segment.        

        \param[out] wayLat The latitude of the intersection point for
        the entrance on the given way.

        \param[out] wayLon The longitude of the intersection point for
        the entrance on the given way.

        \return This method returns true if a valid intersection was
        found.
    */
    bool findPerpendicularIntersection(const Point& entrance,
                                       const Node& node1, const Node& node2,
                                       double& wayLat, double& wayLon) const;

    /** Helper method to find the nearest of two given nodes.

        \param[in] entrance The entrance point of a given building.

        \param[in] node1 The first node for the given way's segment.

        \param[in] node2 The second node for the given way's segment.        

        \param[out] wayLat The latitude of the nearest point for the
        entrance from one of the two nodes.

        \param[out] wayLon The longitude of the nearest point for
        the entrance from one of the two nodes.
    */
    void getShortestDist(const Point& entrance1, const Node& node1,
                         const Node& node2, double& wayLat,
                         double& wayLon) const;

    /** Convenience method compute the entrance to a given building.
        This method first checks to see if any of the nodes for a
        given building-ring are tagged entrances. If not, it returns
        the centroid of the given ring.

        \param[in] bldRing The ring associated with the building whose
        entrance is to be determined.

        \param[in] nodeIDs The list of node IDs associated with the
        vertices of this building.
        
        \return A point indicating the potential entrance to the
        buliding.
    */
    Point findEntrance(const Ring& bldRing,
                       const std::vector<long>& nodeIDs) const;

    /** Helper method to find the Kind of this way based on its way type.

        \param[in] wayType The type of way associated with a given
        way. See https://wiki.openstreetmap.org/wiki/Key:highway

        \return A preset kind for the given type of way.  If the way
        is to be ignored, then this method returns Way::unknown_way
    */
    Way::Kind getWayKind(const std::string& wayType) const;

    /** Helper method to estimate the speed limit for a given way type.

        \param[in] wayType The type of way associated with a given
        way. See https://wiki.openstreetmap.org/wiki/Key:highway

        \return A preset speed limit for the given type of way.  If
        the way is to be ignored, then this method returns zero.
    */
    int estimateSpeedLimit(const std::string& wayType) const;

    /** Change the OSM IDs for nodes (from more-or-less a random
        number) to a consistent zero-based ID value to ease further
        operations and fast look-ups.

        This method changes the OSM ID value of the nodes associated
        with the ways in the OSM.  Currently, the OSM valus are
        more-or-less a randomly assigned number.  We use hash maps to
        manage the look-up. However, hash maps have a high time
        constant. So we re-ID the nodes to a zero-based index value to
        ease look-up using a vector that actually has a very small,
        constant-time look-up.

        \param[out] nodeOrderList The list contains OSM nodeIDs in the
        order in which they have been assigned a zero-based index. The
        actual ID's of the nodes are in their index orders to ease
        writing out nodes in correct order.
    */
    void reIdWayNodes(std::vector<long>& nodeOrderList);

    /** Write the nodes, ways, and building information to a given
        model file.

        \note This method first Re-id's the nodes via call to the
        reIdWayNodes method in this class.
        
        This method uses the write() method in the Node, Way, and
        Building classes to write the data for each node, way, and
        building in the model.
        
        \param[in] filePath The path to the file where the model
        information is to be written.
    */
    void writeModel(const std::string& filePath);

    /** Convenience method to print information about the population
        rings created in the model

        \param[out] os The output stream to where the population
        information is to be written.
    */
    void printPopulationInfo(std::ostream& os = std::cout) const;

    /** Convenience method to print information about the nodes in the
        model.

        \param[out] os The output stream to where the node statistics
        is to be written.
    */
    void printNodesInfo(std::ostream& os = std::cout) const;

    /** Convenience method to check way's that do not have any
        buildings in them and create homes for those ways.

        \param[out] os The output stream to where the empty way
        information is to be written.
    */
    void createHomesOnEmptyWays(std::ostream& os = std::cout);
    
    /** Method to generate homes along a given way separated by given
        spacing and offset from the street at a given depth.

        \param[in,out] way The along with homes are to be generated.
        The building count for this way is updated based on the number
        of homes generated.

        \param[in] spacing The linear spacing between homes in
        miles. The default value of 0.028 maps to 150 feet.

        \param[in] sqFoot The square footage to be associated with the
        home (in square feet).
        
        \param[in] depth The offset from the street where the centroid
        of the home should be located.  The default value of 0.028
        maps to 150 feet.

        \return The number of homes generated.
    */
    int generateHomes(Way& way, const double spacing = 0.014,
                      const double sqFoot = 1000,
                      const double depth = 0.01);

    /** Generate a home on either side of the given latitude and
        longitude.

        \param[in] currNode The current node used to place homes.

        \param[in] nextNode The next node in the way where homes are
        being placed.

        \param[in] homeLat The latitude of the intersection on the way
        where the home is to be placed.

        \param[in] homeLon The intersection 
     */
    void generateHomes(const Node& currNode, const Node& nextNode,
                       const double homeLat, const double homeLon,
                       const double spacing, const double depth,
                       const double sqFoot,  const int wayID);

    /** Draw a generated building.

        This is a helper method that creates rings for buildings and
        adds them to the list of rings to be drawn.

        \param[in] bld The building to be drawn.
    */
    void drawBuilding(const Building& bld);

    /** Distribute the population in each ring to various buildings
        based on square footage.

        This is an internal helper method that is used to distribute
        the population in each population-ring to various buildings in
        the ring.  People are assigned to buildings proportional to
        square footage.
    */
    void distributePopulation();

    /** Write pertinent information about a population ring to a given
        output stream.

        This is a helper method that is used to write the information
        associated with a population ring in a fixed format to a given
        output stream.

        \param[out] os The output stream to where the information
        about this ring is to be written.

        \param[in] idx The index of the population ring in \c popRings
        vector, whose information is to be written.
        
        \param[in] writeHeader If this flag is true then a simple
        comment with the order of the fields is written for future
        reference.
        
        \param[in] delim An optional delimiter between each value
        associated with this node.
    */
    void writePopRing(std::ostream& os, const int idx,
                      const bool writeHeader = false,
                      const std::string& delim = " ") const;

private:
    /** The shape file containing rings loaded from community
        boundaries specified associated with the model.  For example,
        this object contains a set of rings loaded from
        ../boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.shp
        along with annotations loaded from
        ../boundaries/geo_export_79ca7e07-f2ad-4996-9ba2-848b23ace7f5.dbf
    */
    ShapeFile shpFile;

    /** The rings with population information associated with
        approximately 0.57 sq. miles for the various community bounds
        in the shapeFile.  The population is used to determine the
        number of people to be set in each building.
    */
    std::vector<Ring> popRings;

    /** This is a big character buffer that is used to contain the
        parsed XML data loaded from an Open Street Map (OSM) XML file.
        The parsed XML objects are essentially pointers into this
        buffer.

        \note Never directly work this buffer.  Instead always prefer
        to work with the osmXML parsed XML document.
    */
    std::vector<char> osmData;

    /** The top-level OSM XML document that contains a parsed set of
        XML-nodes based on information from the OSM data
    */
    rapidxml::xml_document<> osmXML;

    /** The output file stream to which binary building information is
        written.  Note that each entry in this binary file is a fixed
        building object.  Each building object should be trivially copyable.
    */

    /** This is a simple inner class that is used to conveniently
        encapsulate various command-line arguments that is used by the
        model generator.  This encapsulation streamlines managing
        several different command-line arguments in a streamlined
        manner
    */
    class CmdLineArgs {
    public:
        /** The path to the shape file from where community shapes are
            to be loaded. */
        std::string shapeFilePath;

        /** The path to the DBF file that contains metadata for the
            community shapes. */
        std::string dbfFilePath;

        /** Path to the population raster data from where number of
            people living in a community is to be obtained. */
        std::string popGisFilePath;

        /** Path to the output XFIG figure file to which the model is
            to be written for visual crossvalidation */
        std::string xfigFilePath;
        
        /** The scale for the output XFIG figure so that various
            shapes and information are readable
        */
        int figScale = 4096;

        /** A list of column names in the community shape file that
            are to be included in the XFIG file to ease visual cross
            referencing.
        */
        ArgParser::StringList labelColNames = {"area_numbe", "community"};

        /** Path to the Open Street Map (OSM) XML file from where
            building and street information is to be loaded and
            processed.
        */
        std::string osmFilePath;

        /** The population ring whose detailed buildings are to be
            dumped in the XFIG file.
        */
        int drawPopRingID = -1;

        /** The PUMA ring whose detailed buildings are to be dumped in
            the XFIG file.
        */
        int drawPUMAid = -1;
        
        /** Path to an optional text file with information on
            adjustments to the generated model.  This includes
            buildings to be ignored and remapping of population to
            different grids.

            en generating homes to distribute human population.
            These building IDs are to be ignored because they are not
            tagged correctly in OSM and they significantly throw off
            area calculations.
        */
        std::string adjustmentsFilePath;

        /** Path to an optional model output file where the generated
            model is written for further use/processing. This value is
            set via the \c --out-model command-line argument
        */
        std::string outModelFilePath;

        /** Flag to indicate if buildings generated are to be drawn in
            the output fig file.  The default value is true.  This
            option can be enabled via `--draw-buildings` option.
        */
        bool drawBuildings;

        /** Flag to indicate if road ways from the input OSM XML file must
            be drawn in the output fig file.  This is very useful to
            provide detailed information for verification and
            troubleshooting.  This option can be enabled via `--draw-ways`
            command-line argument.
        */
        bool drawWays;

        /** The directory where the images associated with the output
            fig file are to be cached.  If image tiles are already
            present in the cache directory then they are not
            downloaded.  This makes the process of generating fig
            output significantly faster.  Use the `--cache-dir` to set
            this value.  The default value for this option is
            "../cache"
        */
        std::string cacheDir;

        /** Path to the PUMS people data associated with the region
            for which buildings are being generated.  The data can be
            downloaded from
            https://www.census.gov/programs-surveys/acs/data/pums.html.
            These files are of the format csv_<b>p</b>il.zip -- the
            'p' in the file path is the one that indicates this is a
            people micro sample.  This path is used by the PUMS class
            to load the subset of data necessary for building
            generation.
        */
        std::string pumsPeoplePath;

        /** Path to the PUMS housing data associated with the region
            for which buildings are being generated.  The data can be
            downloaded from
            https://www.census.gov/programs-surveys/acs/data/pums.html.
            These files are of the format csv_<b>h</b>il.zip -- the
            'h' in the file path is the one that indicates this is a
            housing quarters micro sample. This path is used by the
            PUMS class to load the subset of data necessary for
            building generation.
        */
        std::string pumsHousingPath;

        /** Path to the PUMA GIS Shape file. This GIS file provides
            the polygons associated with the PUMA area codes in the
            above 2 PUMS files.  The polygons are used to determine
            overlapping regions for building generation.  This path is
            used by the PUMS class to load the subset of data
            necessary for building generation.
        */
        std::string pumaShpPath;

        /** Path to the PUMA GIS DBF file. This DBF file provides meta
            data about the polygons associated with the PUMA area
            codes in the above 2 PUMS files.  The polygons are used to
            determine overlapping regions for building generation.
            This path is used by the PUMS class to load the subset of
            data necessary for building generation.
        */
        std::string pumaDbfPath;        

        /** The list of column names in PUMS household data to be
            extracted and included in the model.  The order of column
            numbers specified here is preserved in the generated model
        */
        ArgParser::StringList pumsHouColNames;

        /** The list of column names in PUMS people data to be
            extracted and included in the model.  The order of column
            numbers specified here is preserved in the generated model
        */
        ArgParser::StringList pumsPepColNames;

    } cmdLineArgs;

    /** The unordered map that holds information about the nodes
        parsed in from the OSM XML file.  This hash map is populated
        in the extractNodes method.  The hash map enables rapid
        look-up of node information, given the unique ID of the node.
        The look-up is used when processing buildings and processing
        streets.
    */
    std::unordered_map<long, Node> nodeMap;

    /** The unordered map that holds information about the ways in a
        given the OSM XML file.  This hash map is populated in the
        extractWays method.  The hash map enables rapid look-up of
        road information, given the unique ID of a way.  The look-up
        is used when routing traffic.
    */
    std::unordered_map<long, Way> wayMap;

    /** The list of buildings that are generated by the model
        generator.
     */
    std::vector<Building> buildings;

    /** This shapefile is used to draw buildings for a given
        population ring ID.  This shapefile is populated in the
        generate buildings method.

        \note This is tagged mutable so that the constant
        checkExtractBuilding method can add an entry to it. Possibly
        that method should be streamlined to operate differently so
        that we can get rid of the mutable clause on this object.
    */
    mutable ShapeFile buildingShapes;

    /** The set of building IDs to be ignored when generating
        buildings to distribute human population.  These building IDs
        are to be ignored because they are not tagged correctly in OSM
        and they significantly throw off area calculations.
    */
    std::set<long> bldIgnoreIDs;

    /** A look-up table that includes a list of way-IDs for each
        population area.
    */
    std::vector<std::set<long>> popAreaWayIDs;

    /** This is just a copy of the command-line arguments supplied to
        the run method.  This vector is used to write the command-line
        arguments used to generate a model to a given model file.
    */
    std::string actualCmdLineArgs;

    /** Helper class to manage PUMS and PUMA data that is used to
        generate people living in different buildings/houses.
    */
    PUMS pums;
};

#endif
