#ifndef MODEL_GENERATOR_CPP
#define MODEL_GENERATOR_CPP

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

#include <omp.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <unordered_set>
#include "Utilities.h"
#include "ModelGenerator.h"

int
ModelGenerator::run(int argc, char *argv[]) {
    // Save the command-line args for future reference
    auto concat = [](const std::string& s1, const std::string& s2)
        { return s1 + " "  + s2; };
    actualCmdLineArgs = std::accumulate(argv, argv + argc, std::string(""),
                                        concat);
    int error = 0;  // Error from various helper methods.

    // First process the command-line args and ensure we have
    // necessary arguments for performing various operations.
    if ((error = processArgs(argc, argv)) != 0) {
        return error;  // Error processing command-line args.
    }

    // Next load the community shape file
    if ((error = loadShapeFile()) != 0) {
        return error;  // Error loading community shape file.
    }

    // Next load the population data from GIS file and create
    // convenient rings to hold population
    createPopulationRings();
    // Now that we know the number of population rings, resize the
    // popAreaWayIDs vector to have a list of ways per population ring.
    popAreaWayIDs.resize(popRings.size());
    
    // Load the OSM file and collate nodes and generate buildings.
    loadOsmXml();
    extractNodes();
    extractWays();
    // Create homes on the test way
    loadModelAdjustments();
    createBuildings();
    createHomesOnEmptyWays();

    // Distribute people to buildings based on sq.footage
    distributePopulation();
    // Print some statistics about the model
    printPopulationInfo();
    printNodesInfo();

    // Next, generate an output xfig file (if requested)
    generateFig();
    // Finally write the model to file if requested.
    writeModel(cmdLineArgs.outModelFilePath);
    // All done
    return 0;
}

void
ModelGenerator::distributePopulation() {
    for (size_t popID = 0; (popID < popRings.size()); popID++) {
        double pop   = popRings[popID].getPopulation();
        if (pop < 1) {
            continue;  // no people to distribute
        }
        // Find total square footage of all the homes in this ring.
        int popCount = 0; // Population already assigned a building
        
        int bldCount  = 0, homeCount = 0; // not really used
        long homeSqFt = 0, bldSqFt   = 0;
        getTotalSqFootage(popID, bldCount, bldSqFt, homeCount, homeSqFt);
        // Distribute population to buildings proportional to square
        // footage.
        const double sqFtPerPerson = std::floor(homeSqFt / pop);
        // Now distribute the people to each building.
        const int popRingID = popID;
        for (Building& bld : buildings) {
            if (bld.isHome && (bld.attributes == popRingID)) {
                const int levels  = (bld.levels > 1 ? bld.levels : 1);
                const int bldSqFt = bld.sqFootage * levels;
                // Assign pending population to the building.
                bld.population    = std::min(pop, std::ceil(bldSqFt /
                                                            sqFtPerPerson));
                popCount         += bld.population;
                pop              -= bld.population;
            }
        }
        // Print the generated population
        pop = popRings[popID].getPopulation();        
        const double error = (popCount - pop) * 100 / pop;
        std::cout << "Ring " << popRingID << " pop: " << pop
                  << " generated: " << popCount << ", error: "
                  << error << "%\n";
    }
}

void
ModelGenerator::printPopulationInfo(std::ostream& os) const {
    // Assign people to buildings
    os << "ID, population, homeSqFootage, buildings, Sqft/person, info\n";
    for (size_t popID = 0; (popID < popRings.size()); popID++) {
        int blds = 0, pop = popRings[popID].getPopulation(), homes = 0;
        long homeSqFt = 0, bldSqFt = 0;
        getTotalSqFootage(popID, blds, bldSqFt, homes, homeSqFt);
        os << popID << ", " << pop << ", "
           << homeSqFt << ", " << homes << ", "
           << ((pop > 0) ? (homeSqFt / pop) : 0)
           << ", " << popRings[popID].getInfo() << std::endl;
    }
}

void
ModelGenerator::printNodesInfo(std::ostream& os) const {
    // Count the degree distributions of nodes
    std::vector<int> nodeDegrees;
    std::vector<long> exampleNodeID;
    for (auto& entry : nodeMap) {
        const size_t degree = entry.second.refCount;
        if (nodeDegrees.size() <= degree) {
            nodeDegrees.resize(degree + 1);
            exampleNodeID.resize(degree + 1);
        }
        nodeDegrees[degree]++;
        exampleNodeID[degree] = entry.second.osmId;
    }
    // Print the degree distribution
    os << "Degree\t#Nodes\tNodeID\n";
    for (size_t i = 0; (i < nodeDegrees.size()); i++) {
        os << i << '\t' << nodeDegrees.at(i) << '\t'
           << exampleNodeID.at(i) << std::endl;
    }
}

int
ModelGenerator::processArgs(int argc, char *argv[]) {
    // Make the arg_record to process command-line arguments.
    ArgParser::ArgRecord arg_list[] = {
        {"--shape", "The input shapefile to be processed",
         &cmdLineArgs.shapeFilePath, ArgParser::STRING },
        {"--dbf", "The associated DBF file to be used for metadata",
         &cmdLineArgs.dbfFilePath, ArgParser::STRING },        
        {"--xfig", "The output XFig file",
         &cmdLineArgs.xfigFilePath, ArgParser::STRING},
        {"--scale", "The size of the output map",
         &cmdLineArgs.figScale, ArgParser::INTEGER},
        {"--label-cols", "The names of the columns to use for labels",
         &cmdLineArgs.labelColNames, ArgParser::STRING_LIST},
        {"--pop-gis", "GIS data file with human population data",
         &cmdLineArgs.popGisFilePath, ArgParser::STRING},
        {"--osm-xml", "OSM XML file with building & street data",
         &cmdLineArgs.osmFilePath, ArgParser::STRING},
        {"--pop-ring", "ID of population ring to draw buildings",
         &cmdLineArgs.drawPopRingID, ArgParser::INTEGER},
        {"--adjust-model", "Path to file with adjustments to generated model",
         &cmdLineArgs.adjustmentsFilePath, ArgParser::STRING},
        {"--out-model", "Path to file to write generated model",
         &cmdLineArgs.outModelFilePath, ArgParser::STRING},        
        {"", "", NULL, ArgParser::INVALID}
    };
    // Process the command-line arguments.
    ArgParser ap(arg_list);
    ap.parseArguments(argc, argv, true);
    // Ensure at least the shape file is specified.
    if (cmdLineArgs.shapeFilePath.empty()  ||
        cmdLineArgs.popGisFilePath.empty() ||
        cmdLineArgs.osmFilePath.empty()) {
        std::cerr << "Specify a shape file, population GIS file, and OSM XML "
                  << "files to be processed.\n";
        return 1;
    }
    // Things seem fine so far
    return 0;
}

int
ModelGenerator::loadShapeFile() {
    // Load data from the shapefile and dbf file.
    if (!shpFile.loadShapes(cmdLineArgs.shapeFilePath,
                            cmdLineArgs.dbfFilePath)) {
        std::cerr << "Error loading shape/DBF file: "
                  << cmdLineArgs.shapeFilePath << " or "
                  << cmdLineArgs.dbfFilePath   << std::endl;
        return 2;
    }
    // Set default lable information for each entyr in the ring.
    if (!cmdLineArgs.labelColNames.empty()) {
        shpFile.setLabels(cmdLineArgs.labelColNames);
    }
    // Everything went well
    return 0;
}

void
ModelGenerator::createPopulationRings() {
    // Create additional rings from human population, if human
    // population has been specified.
    int minPop = -1, maxPop = -1;
    PopulationRingGenerator prg;
    popRings = prg.createRings(shpFile, cmdLineArgs.popGisFilePath,
                               minPop, maxPop);
    shpFile.addRings(popRings);
    std::cout << "min pop = " << minPop << ", maxPop = " << maxPop
                  << std::endl;
}


void
ModelGenerator::writePopRing(std::ostream& os, const int idx,
                             const bool writeHeader,
                             const std::string& delim) const {
    // Get the population ring whose information is to be written.
    const Ring& pr = popRings.at(idx);
    // Write optional header if requested
    if (writeHeader) {
        os << "# Rng"   << delim << "ID"        << delim << "ringID"   << delim
           << "shpaeID" << delim << "pop"       << delim << "num_vert" << delim
           << "tl_lat"  << delim << "tl_lon"    << delim << "br_lat"   << delim
           << "br_lon"  << delim << "num_bld"   << delim << "bld_sqFt" << delim
           << "homes"   << delim << "home_sqFt" << delim << "info\n";
    }
    // Write the information for this building.
    os << "rng"              << delim << idx                 << delim
       << pr.getRingID()     << delim << pr.getShapeID()     << delim
       << pr.getPopulation() << delim << pr.getVertexCount() << delim;
    // Print the top-left and bottom-right coordinates for this ring,
    // based on the assumption that this ring has been created
    // correctly as per expectations
    ASSERT( pr.getVertexCount() == 5 );
    const Point topLeft = pr.getVertex(0), botRight = pr.getVertex(2);
    // Write the coordinates out (lat lon format)
    const std::streamsize prec =
        os.precision(std::numeric_limits<double>::digits10 + 1);
    os << topLeft.second  << delim  << topLeft.first  << delim
       << botRight.second << delim  << botRight.first << delim;
    os.precision(prec);  // restore previous precision
    // Write number of buildings and total sq.footage of buildings
    int bldCount = 0, homeCount = 0;
    long bldSqFt = 0, homeSqFt  = 0;
    getTotalSqFootage(idx, bldCount, bldSqFt, homeCount, homeSqFt);
    os << bldCount  << delim << bldSqFt  << delim
       << homeCount << delim << homeSqFt << delim;
    // Print a string containing community information
    os << "\"" << pr.getInfo("community") << "\"" << std::endl;
}

void
ModelGenerator::generateFig() {
    // Write the shape information to an XFig file if specified.
    if (!cmdLineArgs.xfigFilePath.empty()) {
        if (cmdLineArgs.drawPopRingID == -1) {
            // The community shapes and population grids are to be drawn
            shpFile.genXFig(cmdLineArgs.xfigFilePath,
                            cmdLineArgs.figScale, true);
        } else {
            // Draw detailed buildings for the given population ring.
            // For this first we create a custom shapeFile and then
            // dump the data.
            buildingShapes.addRing(popRings.at(cmdLineArgs.drawPopRingID));
            // Add ways for this population ring as arcs
            for (int wayID : popAreaWayIDs.at(cmdLineArgs.drawPopRingID)) {
                const Way& way = wayMap.at(wayID);
                buildingShapes.addRing(getRing(way));
            }
            // Generate XFIG
            buildingShapes.genXFig(cmdLineArgs.xfigFilePath,
                                   cmdLineArgs.figScale, true);
        }
    }
}

Ring
ModelGenerator::getRing(const Way& way) const {
    // Create the list of latitudes and longitudes for each node.
    std::vector<double> latList, lonList;
    for (const long nodeID : way.nodeList) {
        const Node& node = nodeMap.at(nodeID);
        latList.push_back(node.latitude);
        lonList.push_back(node.longitude);
    }
    // Now create information associated with the ring.
    const std::vector<Ring::Info> info = {
        {0, "id", std::to_string(way.id)},
        {0, "speed", std::to_string(way.maxSpeed)}
    };
    // Create and return ring.
    return Ring(way.id, -1, Ring::ARC_RING, latList.size(), &lonList[0],
                &latList[0], info);
}

void
ModelGenerator::loadOsmXml() {
    // Read the xml file into a string
    std::cout << "Loading OSM XML from: " << cmdLineArgs.osmFilePath
              << "..." << std::flush;
    std::ifstream xmlFile(cmdLineArgs.osmFilePath);
    if (!xmlFile.good()) {
        throw std::runtime_error("Unable to read data from " +
                                 cmdLineArgs.osmFilePath);
    }
    // Find out the size of the file to pre-allocate string size.
    xmlFile.seekg(0, std::ios::end);
    const int fileSize = xmlFile.tellg();
    xmlFile.seekg(0);  // Reset file pointer -- important!
    
    // Allocate memory to read data
    osmData.resize(fileSize);
    xmlFile.read(&osmData[0], fileSize);
    // Add a '\0' to ensure the string is terminated correctly
    osmData.push_back('\0');

    // Parse the OSM data into an XML document tree
    osmXML.parse<rapidxml::parse_trim_whitespace>(&osmData[0]);
    std::cout << "Done.\n";
}

void
ModelGenerator::extractNodes() {
    // A constant string to ease checks below.
    const std::string NodeName = "node", IDAttr = "id";
    const std::string LatAttr = "lat",   LonAttr = "lon";
    // Find our root node
    rapidxml::xml_node<> * osm = osmXML.first_node("osm");
    std::cout << "Extracting nodes..." << std::flush;
    // Check each XML node and process <node/> entries in parallel.
#pragma omp parallel
    {
        // A per-threads temporary list of nodes to be eventually
        // added to the shared nodeMap hash-map
        std::vector<Node> threadNodeList;
        // Total number of thread running in parallel
        const int numThreads = omp_get_num_threads();
        rapidxml::xml_node<> * node = osm->first_node();
        int nodes2skip = omp_get_thread_num();
        while (node != nullptr) {
            // Skip over nodes that is processed by another thread.
            for (int i = 0; ((node != nullptr) && (i < nodes2skip)); i++) {
                node = node->next_sibling();
            }
            nodes2skip = numThreads;  // From now on skip more nodes.
            if (node == nullptr) {
                break;  // Break out of top-level while loop.
            }
            if (node->name() != NodeName) {
                continue;  // Cannot be a node
            }
            // A convenient temporary Node object
            Node nodeEntry;
            // Extract id, latitude, and longitude into a node object.
            for (rapidxml::xml_attribute<> *attr = node->first_attribute();
                 (attr != nullptr); attr = attr->next_attribute()) {
                if (attr->name() == IDAttr) {
                    nodeEntry.osmId = std::stol(attr->value());
                } else if (attr->name() == LatAttr) {
                    nodeEntry.latitude = std::stod(attr->value());
                } else if (attr->name() == LonAttr) {
                    nodeEntry.longitude = std::stod(attr->value());
                }
            }
            // Extract information if the node could be an entry to a building.
            const KeyValue kv = getKeyValue(node, "entrance", "v");
            if ((kv.first == "entrance") && (kv.second == "yes")) {
                nodeEntry.isEntrance = true;
            }
            // Add parsed node to the per-thread list of nodes
            threadNodeList.push_back(nodeEntry);
        }  // top-level while-loop
        
        // Once all the node entries for this thread have been
        // processed, add the entries to the shared Nodemap.
#pragma omp critical (nodeCS)
        {
            for (const Node& node : threadNodeList) {
                nodeMap[node.osmId] = node;
            }
        }  // end CS
    }  // parallel

    std::cout << "Extracted " << nodeMap.size() << " nodes.\n";
}

void
ModelGenerator::extractWays() {
    // A constant string to ease checks below.
    const std::string ElementWay = "way", IDAttr  = "id";
    const std::string ElementNd  = "nd",  RefAttr = "ref", TagElement = "tag";
    // Track number of ways that have speed set.
    int knowSpeedCount = 0;
    // Find our root node
    rapidxml::xml_node<> * osm = osmXML.first_node("osm");
    std::cout << "Extracting ways..." << std::flush;
    // Check each XML node and process <way/> entries in parallel
#pragma omp parallel
    {
        // A per-threads temporary list of ways to be eventually
        // added to the shared wayMap hash-map
        std::vector<Way> threadWayList;
        // Total number of thread running in parallel
        const int numThreads = omp_get_num_threads();
        rapidxml::xml_node<> * node = osm->first_node();
        int nodes2skip = omp_get_thread_num();
        while (node != nullptr) {
            // Skip over nodes that is processed by another thread.
            for (int i = 0; ((node != nullptr) && (i < nodes2skip)); i++) {
                node = node->next_sibling();
            }
            nodes2skip = numThreads;  // From now on skip more nodes.
            if (node == nullptr) {
                break;  // Break out of top-level while loop.
            }
            if (node->name() != ElementWay) {
                continue;  // Cannot be a way
            }

            // Track all the nodes in a temporary array as we process
            // child elements
            Way wayEntry;
            bool isBuilding = false, isHighway = false;
            int  oneWay     = 0;
            // The following two are used to detect loops (or repeated
            // nodes) in the way.
            bool hasLoop = false;
            std::unordered_set<long> nodeSet;
            std::string wayType;
            for (rapidxml::xml_node<> *child = node->first_node();
                 (child != nullptr); child = child->next_sibling()) {
                if (child->name() == ElementNd) {
                    // This is a node element. Extract the node
                    // reference ID from the XML for future reference
                    ASSERT(child->first_attribute()->name() == RefAttr);
                    long nodeID = std::stol(child->first_attribute()->value());
                    ASSERT(nodeMap.find(nodeID) != nodeMap.end());
                    wayEntry.nodeList.push_back(nodeID);
                    // Check and set loop flag (needed only if we have
                    // not yet found a loop in this way)
                    if (!hasLoop) {
                        hasLoop = (nodeSet.find(nodeID) != nodeSet.end());
                        nodeSet.insert(nodeID);
                    }
                } else if (child->name() == TagElement) {
                    // This is a tag child element. Extract the key,
                    // value attribute to track if this is a building
                    // and speeed limit
                    const KeyValue kv = getKeyValue(child, "k", "v");
                    if (kv.first == "maxspeed") {
                        // Parse out speed limit in the form "45 mph"
                        const std::string& speed = kv.second;
                        wayEntry.maxSpeed =
                            std::stoi(speed.substr(0, speed.find(' ')));
                    } else if ((kv.first == "building") ||
                               (kv.first == "amenity")  ||
                               (kv.first == "building:use")) {
                        isBuilding = true;
                        break;  // This is not a street
                    } else if (kv.first == "highway") {
                        isHighway = true;  // This is a way
                        wayType   = kv.second;  // Type of way
                    } else if (kv.first == "oneway") {
                        if ((kv.second == "yes") || (kv.second == "1") ||
                            (kv.second == "reversible")) {
                            oneWay = 1;
                        } else if (kv.second == "-1") {
                            oneWay = -1;
                        } else if (kv.second == "no") {
                            oneWay = 0;
                        } else {
                            std::cerr << "Unknown oneway value "
                                      << kv.second << " found.\n";
                        }
                    }
                }
            }
            // Check and add an entry for this way
            if (!isBuilding && isHighway && (wayEntry.nodeList.size() > 1)) {
                ASSERT(node->first_attribute()->name() == IDAttr);
                wayEntry.id = std::stol(node->first_attribute()->value());
                // If the oneWay flag is set to -1, then we need to
                // reverse the direction of the nodes to get the
                // direction corrected.
                if (oneWay == -1) {
                    std::reverse(wayEntry.nodeList.begin(),
                                 wayEntry.nodeList.end());
                }
                // Setup the direction flag
                wayEntry.isOneWay = (oneWay != 0);
                // Setup the kind for this way
                wayEntry.kind = getWayKind(wayType);
                // Setup the speed limit if it is not already set.
                if (wayEntry.maxSpeed == -1) {
                    wayEntry.maxSpeed = estimateSpeedLimit(wayType);
                }
                // Setup loop flag
                wayEntry.hasLoop = hasLoop;
                // Finally add the way entry, if it is meaningful
                if (wayEntry.maxSpeed > 0) {
                    // Cross reference the way with population rings
                    if (addWayToPopRings(wayEntry)) {
                        // Add entry to the Ways hash map
                        threadWayList.push_back(wayEntry);
                    }
                }
            }
        }

#pragma omp critical
        {
            for (const Way& way : threadWayList) {
                // Add ways to the global map to ease look-up
                wayMap[way.id] = way;
                // Update reference counters on the nodes of the
                // way. We need to do this in the critical section
                // because nodes can be part of multiple ways.
                for (const long nodeID : way.nodeList) {
                    nodeMap[nodeID].refCount++;
                }
                // Track ways for which the speed limit is known
                if (way.maxSpeed != -1) {
                    knowSpeedCount++;   // We know the speed
                }
            }
        }  // critical

        // Wait for all the threads to finish updating the node counts
        // for the ways.
#pragma omp barrier        
        // Now we can tell if a given way is a dead end or an
        // intersection.  This is useful for routing traffic later on.
        int deadEnds = 0;
        for (const Way& wayRef : threadWayList) {
            // Get the way in the global map
            Way& way = wayMap.at(wayRef.id);
            int intersections = 0;  // nodes with degree > 1
            for (const long nodeID : way.nodeList) {
                if (nodeMap.at(nodeID).refCount > 1) {
                    intersections++;
                }
            }
            // dead-ends have only one node with degree > 1
            way.isDeadEnd = (intersections < 2);
            if (way.isDeadEnd) {
                deadEnds++;
            }
        }

        std::cout << "Number of dead ends: " << deadEnds << std::endl;
    }  // parallel
    std::cout << "Extracted " << wayMap.size() << " ways. "
              << knowSpeedCount << " ways have speed limit set.\n";
}

int
ModelGenerator::estimateSpeedLimit(const std::string& wayType) const {
    // The speed limits set for different types of ways in OSM, if a
    // max speed limit is not already specified.
    static const std::unordered_map<std::string, int> SpeedLimits = {
        {"service", 25}, {"residential", 25}, {"primary", 35},
        {"secondary", 35}, {"tertiary", 25}, {"motorway_link", 45},
        {"motorway", 65}, {"trunk", 55}, {"primary_link", 45},
        {"trunk_link", 45}, {"secondary_link", 25}, {"tertiary_link", 25},
        {"unclassified", 10}};
    // Check if the wayType is one assigned
    const auto entry = SpeedLimits.find(wayType);
    if (entry != SpeedLimits.end()) {
        return entry->second;  // return speed limit
    }
    // This is an entry to be ignored
    // std::cout << "Way type " << wayType << " ignored.\n";
    return 0;
}

Way::Kind
ModelGenerator::getWayKind(const std::string& wayType) const {
    // The speed limits set for different types of ways in OSM, if a
    // max speed limit is not already specified.
    static const std::unordered_map<std::string, Way::Kind> WayKinds = {
        {"service",  Way::service},  {"residential",   Way::residential},
        {"primary",  Way::primary},  {"secondary",     Way::secondary},
        {"tertiary", Way::tertiary}, {"motorway_link", Way::motorway_link},
        {"motorway", Way::motorway}, {"trunk",         Way::trunk},
        {"primary_link",   Way::primary_link}, {"trunk_link", Way::trunk_link},
        {"secondary_link", Way::secondary_link},
        {"tertiary_link",  Way::tertiary_link}};
    // Check if the wayType is one assigned
    const auto entry = WayKinds.find(wayType);
    if (entry != WayKinds.end()) {
        return entry->second;  // return kind for this way.
    }
    // This is a Way type that we don't care about.
    return Way::unknown_way;
}

KeyValue
ModelGenerator::getKeyValue(const rapidxml::xml_node<>* node,
                            const std::string& KeyAttr,
                            const std::string& ValAttr) const {
    KeyValue retVal;
    for (rapidxml::xml_attribute<> *attr = node->first_attribute();
         (attr != nullptr); attr = attr->next_attribute()) {
        if (attr->name() == KeyAttr) {
            retVal.first = attr->value();
        } else if (attr->name() == ValAttr) {
            retVal.second = attr->value();
        }
    }
    return retVal;
}

int
ModelGenerator::getPopRing(const Ring& ring) const {
    // Get the centroid for searching.
    const Point centroid = ring.getCentroid();
    // Use helper method to find the population ring
    return getPopRing(centroid.second, centroid.first);
}

int
ModelGenerator::getPopRing(const double latitude,
                           const double longitude) const {
    // Search each one of the population rings
    for (size_t i = 0; (i < popRings.size()); i++) {
        if (popRings[i].contains(longitude, latitude)) {
            return i;  // Found a ring for this building
        }
    }
    return -1;  // Point not in region of interest
}

void
ModelGenerator::createBuildings() {
    // Some of the constant strings used to streamline code below.
    const std::string Way = "way";
    // Find our root node
    rapidxml::xml_node<> * osm = osmXML.first_node("osm");
    std::cout << "Extracting buildings..." << std::flush;
    // Check each XML node and process <way/> entries for buildings parallel.
#pragma omp parallel
    {
        // A per-threads temporary list of building nodes to be
        // eventually added to the shared buildings vector
        std::vector<Building> threadBuildingList;
        // per-thread latitude and longitude values for vertices
        // constituting the building.  This list is reused to keep
        // code fast.
        std::vector<double> vertexLat, vertexLon;
        // Total number of thread running in parallel
        const int numThreads = omp_get_num_threads();
        int nodes2skip       = omp_get_thread_num();
        // Iterate over all nodes and extract buildings
        rapidxml::xml_node<> * node = osm->first_node();
        while (node != nullptr) {
            // Skip over nodes that is processed by another thread.
            for (int i = 0; ((node != nullptr) && (i < nodes2skip)); i++) {
                node = node->next_sibling();
            }
            nodes2skip = numThreads;  // From now on skip more nodes.
            if (node == nullptr) {
                break;  // Break out of top-level while loop.
            }
            if (node->name() != Way) {
                continue;  // Cannot be a building node
            }

            // Use a helper method to process the XML element and
            // return a valid building entry, if the node is a valid
            // building.
            Building bld = checkExtractBuilding(node, vertexLat, vertexLon);
            if (bld.id != 0) {
                threadBuildingList.push_back(bld);
            }
        }

        // Now that all the nodes have been processed, add the
        // per-thread-list of bulidings to the global list.
#pragma omp critical
        {
            // Add buildings to the global list.
            buildings.insert(buildings.end(), threadBuildingList.begin(),
                             threadBuildingList.end());
            // Update the number of buildings in each way.
            for (Building& bld : threadBuildingList) {
                if (bld.isHome) {
                    wayMap.at(bld.wayID).numBuildings++;
                }
            }
        }
    }  // parallel
    
    std::cout << "Created " << buildings.size() << " buildings.\n";    
}

// NOTE: This method is called from multiple threads.
bool
ModelGenerator::processBuildingElements(rapidxml::xml_node<>* node,
                                        std::vector<double>& vertexLat,
                                        std::vector<double>& vertexLon,
                                        std::string& type, int& levels,
                                        std::vector<long>& nodes,
                                        bool& isHome) const {
    const std::string BldNode = "nd", Tag = "tag", Ref = "ref";
    const std::string KeyAttr = "k", ValAttr = "v", IDAttr = "id";
    const std::string HomeTypes = "yes house residential apartments "
        "condominium";
    // The following types of buildings are ignored and not
    // included.
    const std::string IgnoreTypes = "industrial terrace garages warehouse "
        "shed root construction manufacture";
    // Set out variables to default initial values.
    vertexLat.clear();
    vertexLon.clear();
    nodes.clear();
    type   = "n/a";
    levels = -1;
    // Local flags updated in the for-loop below.
    bool isBuilding = false, isAmenity = false;
    // Iterate over all the child nodes.
    for (rapidxml::xml_node<> *child = node->first_node();
         (child != nullptr); child = child->next_sibling()) {
        if (child->name() == BldNode) {
            // This a node/vertex for a building.
            ASSERT( child->first_attribute()->name() == Ref );
            // Get the node's latitude and longitude information
            const long nodeID  = std::stol(child->first_attribute()->value());
            const Node& vertex = nodeMap.at(nodeID);
            // Track the vertices for this building.
            nodes.push_back(nodeID);
            vertexLat.push_back(vertex.latitude);
            vertexLon.push_back(vertex.longitude);
        } else if (child->name() == Tag) {
            const KeyValue kv = getKeyValue(child, KeyAttr, ValAttr);
            if (kv.first == "building") {
                type = kv.second;
                isBuilding = true;
            } else if (kv.first == "amenity") {
                isAmenity  = true;
            } else if (kv.first == "building:use") {
                isAmenity = HomeTypes.find(kv.second) == std::string::npos;
            } else if (kv.first == "building:levels") {
                try {
                    levels = std::stoi(kv.second);
                } catch (const std::invalid_argument& ia) {
                    std::cerr << "Unable to convert level " << kv.second
                              << std::endl;
                }
            }
        }
    }

    // Now that we have processed all the building's attributes,
    // it is time to create a new building object if the
    // conditions are met and the building is a closed polygon
    if (!isBuilding || isAmenity || (vertexLat.size() < 3) ||
        (vertexLat.front() != vertexLat.back())) {
        return false;  // This is not a valid buliding at all.
    }
    // Check and ignore industrial buildings
    if (IgnoreTypes.find(type) != std::string::npos) {
        return false;  // This type of building is to be ignored
    }
    // Check to see if this is a valid residential building.
    isHome = (HomeTypes.find(type) != std::string::npos);
    // Found a valid building
    return true;
}
                                             
// NOTE: This method is called from multiple threads.
Building
ModelGenerator::checkExtractBuilding(rapidxml::xml_node<>* node,
                                     std::vector<double>& vertexLat,
                                     std::vector<double>& vertexLon) const {
    const std::string BldNode = "nd", Tag = "tag", Ref = "ref";
    const std::string KeyAttr = "k", ValAttr = "v", IDAttr = "id",
        VerAttr = "version";    
    // We have to check all the attributes for the building before we
    // can decide if this is a home/apartment.  In addition we need to
    // check to see if this way is tagged as a building or an
    // apartment but not an amenity by checking child XML "tag"
    // elements.
    Building invalidBld;  // This is the building object to be returned.
    // Extract the unique IDs for the building
    const KeyValue id = getKeyValue(node, IDAttr, VerAttr);
    const int bldID   = std::stol(id.first);
    if (bldIgnoreIDs.find(bldID) != bldIgnoreIDs.end()) {
        return invalidBld;  // This building is to be ignored.
    }
    // Get a helper method to process the building data first.
    std::string buildingType;
    int levels = -1;
    bool isHome = false;
    std::vector<long> nodes;
    if (!processBuildingElements(node, vertexLat, vertexLon,
                                 buildingType, levels, nodes, isHome)) {
        return invalidBld;  // Invalid building
    }
    // This is a valid building.  For convenience we create a
    // building using a static helper method.
    ASSERT(vertexLat.size() > 2);
    Ring ring(bldID, -1, Ring::BUILDING_RING, vertexLat.size(),
              &vertexLon[0], &vertexLat[0], {
                  {0, "bldID", std::to_string(bldID)}});
    if (ring.getArea() < 1.8e-05) {
        // Area is less than 500 sq.foot. Ignore this building.  With
        // 500 sq.foot threshold we generate 602393 building for
        // Chicago metro area.  With 100 sq. foot threshold we
        // generate 806201 buildings with many tiny buildings.
        return invalidBld;
    }
    const int popRingID = getPopRing(ring);
    if (popRingID == -1) {
        // Ignore this building as it is not in the community areas we
        // are interested in.
        return invalidBld;
    }

    // Compute the nearest way and intersection for the building.
    Building bld = Building::create(ring, bldID, levels, 0, popRingID);
    bld.isHome   = isHome;
    bld.wayID = findNearestIntersection(ring, popRingID, nodes,
                                        bld.wayLat, bld.wayLon);
    if (bld.wayID == -1) {
        // Could not find an entrace to a given building.
        return invalidBld;
    }
    // Add building to be drawn based on command-line args
    if (cmdLineArgs.drawPopRingID == popRingID) {
        // Entrance to draw in the figure.
        const Point entrance = findEntrance(ring, nodes);
        const std::vector<double> xCoords = {entrance.first,  bld.wayLon};
        const std::vector<double> yCoords = {entrance.second, bld.wayLat};
        const std::string entInfo         = std::to_string(bld.id) + "--" +
            std::to_string(bld.wayID);
        const std::vector<Ring::Info> info= {
            {1, "info",  entInfo }
        };
        const Ring entryArc = Ring(bldID, bldID, Ring::ENTRY_RING, 2,
                                   &xCoords[0], &yCoords[0], info);
#pragma omp critical
        {
            buildingShapes.addRing(ring);
            buildingShapes.addRing(entryArc);
        }
    }
    // Return a valid building entry for further use.
    return bld;
}

void
ModelGenerator::getTotalSqFootage(const int popRingID, int& bldCount,
                                  long& bldSqFt, int& homeCount,
                                  long& homeSqFt) const {
    bldSqFt   = 0;
    homeSqFt  = 0;
    bldCount  = 0;
    homeCount = 0;
    // Iterate overall the buildings and tracks buildings associated
    // with the given population ring.
    for (const Building& bld : buildings) {
        if (bld.attributes != popRingID) {
            continue;  // Building is not in the ring of interest
        }
        ASSERT(bld.sqFootage > 0);
        bldCount++;
        const long sqFt = std::max(1, bld.levels) * bld.sqFootage;
        bldSqFt += sqFt;
        // Count number of homes.
        if (bld.isHome) {
            homeCount++;
            homeSqFt += sqFt;
        }
    }
    ASSERT( homeCount <= bldCount );
    ASSERT( homeSqFt  <= bldSqFt  );
}

void
ModelGenerator::loadModelAdjustments() {
    if (cmdLineArgs.adjustmentsFilePath.empty()) {
        return;  // Ignore file not specified. Nothign to do.
    }
    // Check to ensure the specified ignroe file is valid.
    std::ifstream ignoreFile(cmdLineArgs.adjustmentsFilePath);
    if (!ignoreFile.good()) {
        throw std::runtime_error("Error reading " +
                                 cmdLineArgs.adjustmentsFilePath);
    }
    // Process line-by-line from the line. Lines starting with '#' or
    // blank lines are comments and are ignored.
    std::string line;
    while (std::getline(ignoreFile, line)) {
        line = trim(line);  // remove leading/trailing blanks
        if (line.empty() || (line[0] == '#')) {
            continue;  // blank or comment line
        }
        // Read the first word of the line which should be the type of
        // information on that line
        const size_t spcPos   = line.find(' ');
        const std::string cmd = line.substr(0, spcPos);
        // Remove 1st word from line for convenient processing below.
        line                  = trim(line.substr(spcPos + 1));
        if (cmd == "ignore") {
            // Adjustment to ignore buildings when distributing
            // population.  If the line as a ":" then it is a range of
            // numbers and not just one. So handle that situation
            // correctly.
            const size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                // This is a line with exactly 1 buliding ID. 
                bldIgnoreIDs.insert(std::stol(line));
            } else {
                // This line has a range of building IDs in it. Add
                // individual IDs to the set of buildings to be ignored.
                const int startID = std::stol(line.substr(0, colonPos));
                const int endID   = std::stol(line.substr(colonPos + 1));
                // Add each buliding ID in the range to our ignore set
                ASSERT( startID <= endID);
                for (long id = startID; (id <= endID); id++) {
                    bldIgnoreIDs.insert(id);
                }
            }
        } else if (cmd == "remap") {
            // This is option to remap population from one ring to
            // another in the form "512 513".  Read the source and
            // destination ring indexes using an string stream.
            std::istringstream is(line);
            int srcRingIdx, destRingIdx;
            is >> srcRingIdx >> destRingIdx;
            // Check to ensure source and destination ring indexes
            // look correct.
            if ((srcRingIdx  < 0)  || (srcRingIdx  >= (int) popRings.size()) ||
                (destRingIdx < -1) || (destRingIdx >= (int) popRings.size())) {
                throw std::runtime_error("Invalid source/destination ring "
                                         "indexes on line " + line);
            }
            // Now move population from source to destination ring if
            // destination ring is not -1.
            if (destRingIdx != -1) {
                const double pop = popRings[destRingIdx].getPopulation() +
                    popRings[srcRingIdx].getPopulation();
                popRings[destRingIdx].setPopulation(pop);
            }
            // Clear out population in the source ring.
            popRings[srcRingIdx].setPopulation(0);
        }
    }
    // Finally print some informational message
    std::cout << "Loaded " << bldIgnoreIDs.size() << " buildings "
              << "to be ignored.\n";
}

// NOTE: This method is called from multiple threads.
bool
ModelGenerator::addWayToPopRings(const Way& way) {
    // Check each node in the way. Note that a long way can belong to
    // many population rings.  So we have to check every point.  NOTE:
    // This approach cannot correctly detect very long (say over 0.57
    // mile straint line stretch) roads cutting across a population
    // ring.
    std::set<int> ringIDs;  // Local ring IDs
    for (const long nodeID : way.nodeList) {
        // Get the node entry corresponding to the nodeID
        const Node node = nodeMap.at(nodeID);
        // Search for a ring that could contain this way.
        for (size_t rngIdx = 0; (rngIdx < popRings.size()); rngIdx++) {
            const Ring& ring = popRings[rngIdx];
            if (ring.contains(node.longitude, node.latitude) ||
                ring.isNear(node.longitude, node.latitude)) {
                // Found a population ring that contains this
                // way. Track it.
                ringIDs.insert(rngIdx);
            }
        }
    }  // check each point in the way.

    // If we did not find any population rings for this way then
    // there is nothing much to do.
    if (ringIDs.empty()) {
        return false;
    }

    // Finally add the list of rings to the global list of popAreaWayID
#pragma omp critical
    {
        for (int rngIdx : ringIDs) {
            popAreaWayIDs[rngIdx].insert(way.id);
        }
    }
    // We found at least 1 population ring. This way should be tracked
    // and used.
    return true;
}

long
ModelGenerator::findNearestIntersection(const Ring& bldRing,
                                        const int popRingID,
                                        const std::vector<long>& nodeList,
                                        double& wayLat, double& wayLon) const {
    // Find the entrance (most likely the centroid of this building)
    const Point entrance = findEntrance(bldRing, nodeList);
    // Find the way in the given populationRing with shortest
    // perpendicular distance to the entrance
    long   nearWayID = -1;
    double minDist   = 1000;
    wayLat = wayLon  = -1;
    // We only iterate over ways in the population ring as they would
    // be the nearest and keeps computation to a minimum
    for (const int wayID : popAreaWayIDs.at(popRingID)) {
        // Find the minimum perpendicular distance to this way.
        const Way& way = wayMap.at(wayID);
        double interWayLat = -1, interWayLon = -1;
        const double dist = findNearestIntersection(entrance, way, interWayLat,
                                                    interWayLon);
        if (dist < minDist) {
            // This is the first way or we found a shorter
            // intersection/way.  Track the new information.
            nearWayID = way.id;
            minDist   = dist;
            wayLat    = interWayLat;
            wayLon    = interWayLon;
        }
    }
    // Check to ensure we have a way and distance to return
    if ((nearWayID == -1) || (minDist > 0.1)) {
        std::cout << "Unable to find a good street intersection for "
                  << "building " << bldRing.getRingID() << " -- minDist = "
                  << minDist << ", popRingID = " << popRingID << std::endl;
    }    
    // Return the nearest way ID we found.
    return nearWayID;
}

double
ModelGenerator::findNearestIntersection(const Point& entrance, const Way& way, 
                                        double& wayLat, double& wayLon) const {
    double minDist  = 1000;
    wayLat = wayLon = -1;
    for (size_t i = 1; (i < way.nodeList.size()); i++) {
        // Get the two vertices for this segment of the way
        const Node& node1 = nodeMap.at(way.nodeList[i - 1]);
        const Node& node2 = nodeMap.at(way.nodeList[i]);
        // Check to ensure the entrace is between the two
        // nodes. Otherwise the perpendicular distance will not mean
        // much.
        double interLon = entrance.first, interLat = entrance.second;
        // Compute x or y intercept with the way depending on location
        if (!findPerpendicularIntersection(entrance, node1, node2,
                                           interLat, interLon)) {        
            if (inBetween(node1.latitude,  node2.latitude,  entrance.second)) {
                // Point is between the y-coordinates. So retain y-coordinate
                // of entrance and find the x-coordinate on the way
                const double slope = (node2.longitude - node1.longitude) /
                    (node2.latitude - node1.latitude);
                const double offset = node2.latitude -
                    (node2.longitude / slope);
                interLat = entrance.first;
                interLon = (interLat - offset) * slope;
            } else if (inBetween(node1.longitude, node2.longitude,
                                 entrance.first)) {
                // Point is between the x-coordinates. So retain
                // longitude and find the latitude
                const double slope = (node2.latitude - node1.latitude) /
                    (node2.longitude - node1.longitude);
                const double offset = node2.latitude -
                    (node2.longitude * slope);
                interLon = entrance.second;
                interLat = (slope * interLon) + offset;
            } else {
                // A perpendicular (or shortest path) to the
                // way-segment is not feasible. So just check the two
                // ends and pick the shortest of the two.
                getShortestDist(entrance, node1, node2, interLat, interLon);
            }
        }
        // Next compute the distance between the entrance and the
        // intercept on the way.
        const double distance = getDistance(entrance.second, entrance.first,
                                            interLat, interLon);
        // Track first/nearest distance
        if (distance < minDist) {
            // Found a better estimate
            minDist = distance;
            wayLat  = interLat;
            wayLon  = interLon;
        }
    }
    // Return the minimum distance found
    return minDist;
}

void
ModelGenerator::getShortestDist(const Point& entrance, const Node& node1,
                                const Node& node2, double& wayLat,
                                double& wayLon) const {
    // Compute distances to the two ends of the way-segment from the entrance.
    const double dist1 = getDistance(entrance.second, entrance.first,
                                     node1.latitude,  node1.longitude);
    const double dist2 = getDistance(entrance.second, entrance.first,
                                     node2.latitude,  node2.longitude);
    // Pick the node based on the distance
    if (dist1 < dist2) {
        wayLat = node1.latitude;
        wayLon = node1.longitude;
    } else {
        wayLat = node2.latitude;
        wayLon = node2.longitude;
    }    
}

// Wrapper to find perendicular distance
bool
ModelGenerator::findPerpendicularIntersection(const Point& entrance,
                                              const Node& node1,
                                              const Node& node2,
                                              double& wayLat,
                                              double& wayLon) const {
    // Use helper method in Utilities to compute perpendicular intersection
    return ::findPerpendicularIntersection(entrance.second, entrance.first,
                                           node1.latitude, node1.longitude,
                                           node2.latitude, node2.longitude,
                                           wayLat, wayLon);
}

// NOTE: This method is called from multiple threads
Point
ModelGenerator::findEntrance(const Ring& bldRing,
                             const std::vector<long>& nodeIDs) const {
    // First check to see if any of the nodes are tagged as
    // entrance. If so use that node's latitude and longitude as the
    // entrance to the building.
    for (const long id : nodeIDs) {
        const Node& node = nodeMap.at(id);
        if (node.isEntrance) {
            // Found a tagged entrance. Give preference to this one.
            return Point(node.longitude, node.latitude);
        }
    }
    // No tagged entrances were found. In this case just return the
    // centroid of the ring as the suggested entrance location
    return bldRing.getCentroid();
}

void
ModelGenerator::reIdWayNodes(std::vector<long>& nodeOrderList) {
    // A convenience map from osmID -> zero-based-index for each node.
    std::unordered_map<long, int> idIdMap;
    // For each node in a way generate zero-based IDs to enable faster
    // look-ups during route processing.
    for (auto& wayEntry : wayMap) {
        // Get the way whose nodes are to be re-ID'd
        Way& way = wayEntry.second;
        // Process each node in the way.
        for (long& nodeID : way.nodeList) {
            // Convert the nodeID from OSM id to zero-based index
            auto idEntry = idIdMap.find(nodeID);
            // Handle the 2 cases -- node has already been ID'd or
            // this is a new node to be ID'd
            if (idEntry == idIdMap.end()) {
                // This node has not been mapped. Do it now.
                const long newID = nodeOrderList.size();
                idIdMap[nodeID]  = newID;
                // Save the order of nodes for future reference
                nodeOrderList.push_back(nodeID);
                // Update the new zero-based ID for this node.
                nodeID = newID;
            } else {
                // We already have a zero-based ID for this node.
                nodeID = idEntry->second;
            }
        }
    }
}

void
ModelGenerator::writeModel(const std::string& filePath) {
    if (filePath.empty()) {
        return;  // nothing further to do.
    }
    // Open output file and check to ensure it is ok.
    std::ofstream model(filePath);
    if (!model.good()) {
        throw std::runtime_error("Error writing to model file " + filePath);
    }
    // Re-ID nodes to make them have contiguous IDs to make way
    // processing eaiser.
    std::vector<long> nodeOrderList;  // Node order list
    reIdWayNodes(nodeOrderList);      // Re-ID nodes to ease processing

    // Write some metadata about the model
    model << "# Model created at: "   << getSystemTime()
          << "# Command-line used: "  << actualCmdLineArgs    << "\n"
          << "# Rings in model: "     << popRings.size()      << "\n"
          << "# Nodes in model: "     << nodeOrderList.size() << "\n"
          << "# Buildings in model: " << buildings.size()     << "\n"
          << "# Ways in model: "      << wayMap.size()        << "\n\n"
          << std::boolalpha;

    // Write the population rings in the model
    for (size_t i = 0; (i < popRings.size()); i++) {
        writePopRing(model, i, (i == 0));
    }
    model << std::endl; // Empty line after all the population rings
    
    // Write the nodes in the model in the correct order.
    for (const long nodeID : nodeOrderList) {
        // Get the node and have the node write information about
        // itself.
        const Node& node = nodeMap.at(nodeID);
        node.write(model, nodeID == nodeOrderList.front());
     }
    // Next write the ways in the model
    model << "\n# Ways in model: " << wayMap.size() << std::endl;
    const long firstWayID = wayMap.cbegin()->first;
    for (const auto entry : wayMap) {
        entry.second.write(model, entry.first == firstWayID);
    }
    // Finally write the buildings in the model
    model << "\n# Buildings in model: " << buildings.size() << std::endl;    
    const long firstBldID = buildings.front().id;
    for (const Building& bld : buildings) {
        bld.write(model, bld.id == firstBldID);
    }    
}

void
ModelGenerator::createHomesOnEmptyWays(std::ostream& os) {
    // Print information on way's that do not have any buildings.
    int emptyWayCount = 0, genHomeCount = 0;
    for (auto& entry : wayMap) {
        // Get the way for convenience.
        Way& way = entry.second;
        if ((way.kind != Way::residential) || (way.numBuildings > 0)) {
            continue;  // Not a residential way or it has buildings
        }
        // Found an empty way. Generate buildings.
        emptyWayCount++;
        // Generate homes 
        const int genHomes = generateHomes(way);
        genHomeCount      += genHomes;
        os << "Way #" << way.id << " is empty. Generated " << genHomes
           << " homes.\n";
    }
    // Finally print number of empty ways.
    os << "Out of " << wayMap.size() << " ways, "  << emptyWayCount
       << " residential ways had 0 buildings and " << genHomeCount
       << " total homes have been generated.\n";
}

int
ModelGenerator::generateHomes(Way& way, const double spacing,
                              const double sqFoot, const double depth) {
    // Curr node is the current lat,lon point where the home will be
    // created. This value is updated as homes are added.
    Node currNode = nodeMap.at(way.nodeList[0]);
    Node nextNode = nodeMap.at(way.nodeList[1]);
    // Track the space left between the current and the next node.
    double spaceLeft = getDistance(currNode.latitude, currNode.longitude,
                                   nextNode.latitude, nextNode.longitude);
    // The index of the node up to which we have calculated spaceLeft
    size_t nodeIdx = 1;  // Index of current node in the way
    // Create homes along the way, while updating various variables.
    do {
        // Check to see if we have enough space to create a home. If
        // not move onto the next point.
        if ((spaceLeft <= spacing) && (nodeIdx < way.nodeList.size())) {
            if (++nodeIdx < way.nodeList.size()) {
                // Onto next node on the way to get more space.
                nextNode = nodeMap.at(way.nodeList[nodeIdx]);
                spaceLeft += getDistance(currNode.latitude, currNode.longitude,
                                         nextNode.latitude, nextNode.longitude);
            }
        } else {
            // We have space to fit a home. Find the point of the home
            // along the current way.
            double homeLat, homeLon;
            getPoint(currNode.latitude, currNode.longitude, nextNode.latitude,
                     nextNode.longitude, spacing / 2, homeLat, homeLon);
            if (inBetween(currNode.latitude, nextNode.latitude, homeLat)   &&
                inBetween(currNode.longitude, nextNode.longitude, homeLon) &&
                (getPopRing(homeLat, homeLon) != -1)) {
                // Create home on either side of this way using
                // helper method.
                generateHomes(currNode, nextNode, homeLat, homeLon,
                              spacing, depth, sqFoot, way.id);
                way.numBuildings += 2;
                // Move the currNode to the next home location and
                // decrease the space left.
                getPoint(currNode.latitude, currNode.longitude,
                         nextNode.latitude, nextNode.longitude, spacing,
                     currNode.latitude, currNode.longitude);
            }
            spaceLeft -= spacing;
        }
    } while ((spaceLeft >= spacing) || (nodeIdx < way.nodeList.size()));

    // Return the number of buildings created for this way
    return way.numBuildings;
}

void
ModelGenerator::generateHomes(const Node& currNode, const Node& nextNode,
                              const double homeLat, const double homeLon,
                              const double spacing, const double depth,
                              const double sqFoot,  const int wayID) {
    // To get the corners of the building we need to calcluate the
    // offset of the building from the way. Compute the delta in
    // points's latitude and longitude to ease rotation around the
    // home's latitude and longitude
    const double homeWidth = std::sqrt(sqFoot) * 0.0001894;
    const double homeDepth = sqFoot * 3.58701e-8 / homeWidth;
    double home1Lat, home1Lon, home2Lat, home2Lon, homeSpcLat, homeSpcLon;
    getPoint(homeLat, homeLon, nextNode.latitude, nextNode.longitude, depth,
             home1Lat, home1Lon, true);
    getPoint(homeLat, homeLon, nextNode.latitude, nextNode.longitude,
             depth + homeDepth, home2Lat, home2Lon, true);
    getPoint(homeLat, homeLon, nextNode.latitude, nextNode.longitude,
             homeWidth / 2, homeSpcLat, homeSpcLon, true);
    // Find the population ring for this home
    const int popRingID = getPopRing(homeLat, homeLon);
    // Now create a building around the centroid with given sqFootage
    Building home1 = Building::create(buildings.size(), sqFoot, wayID, homeLat,
                                      homeLon, homeLat - home1Lon - homeSpcLat,
                                      homeLon + home1Lat - homeSpcLon,
                                      homeLat - home2Lon + homeSpcLat,
                                      homeLon + home2Lat + homeSpcLon,
                                      popRingID);
    Building home2 = Building::create(buildings.size() + 1, sqFoot, wayID,
                                      homeLat, homeLon,
                                      homeLat + home1Lon - homeSpcLat,
                                      homeLon - home1Lat - homeSpcLon,
                                      homeLat + home2Lon + homeSpcLat,
                                      homeLon - home2Lat + homeSpcLon,
                                      popRingID);
    // Add the created buildings to the list
    buildings.push_back(home1);
    buildings.push_back(home2);
    // Draw the building if this ring is to be drawn
    if (cmdLineArgs.drawPopRingID == popRingID) {
        drawBuilding(home1);
        drawBuilding(home2);
    }
}

void
ModelGenerator::drawBuilding(const Building& bld) {
    // Create a ring for the boundaries of this building.
    const std::vector<double> xCoords =
        {bld.topLon, bld.botLon, bld.botLon, bld.topLon, bld.topLon};
    const std::vector<double> yCoords =
        {bld.topLat, bld.topLat, bld.botLat, bld.botLat, bld.topLat};
    const std::string entInfo = std::to_string(bld.id) + "--" +
        std::to_string(bld.wayID);
    const std::vector<Ring::Info> info= {{1, "info",  entInfo }};
    // Create a ring for this building.
    const Ring bldRing = Ring(bld.id, bld.id, Ring::BUILDING_RING, 5,
                              &xCoords[0], &yCoords[0], info);
    // Create an entry ring for this building.
    const Point ent = bldRing.getCentroid();
    const std::vector<double> entXCoords = {ent.first,  bld.wayLon};
    const std::vector<double> entYCoords = {ent.second, bld.wayLat};
    const Ring entryArc = Ring(bld.id, bld.id, Ring::ENTRY_RING, 2,
                               &entXCoords[0], &entYCoords[0], info);
    // Add shapes to be drawn
    buildingShapes.addRing(bldRing);
    buildingShapes.addRing(entryArc);
}

#endif
