#ifndef WAY_RISK_ANALYZER_CPP
#define WAY_RISK_ANALYZER_CPP

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

#include <numeric>
#include <chrono>
#include "Utilities.h"
#include "WayRiskAnalyzer.h"
#include "PathFinder.h"
#include "Stopwatch.h"

int
WayRiskAnalyzer::processArgs(int argc, char *argv[]) {
    // Save the command-line arguments for future reference.
    auto concat = [](std::string s1, std::string s2) { return s1 + " " + s2; };
    cmdLineArgs.fullCmdLine = std::accumulate(argv, argv + argc,
                                              cmdLineArgs.fullCmdLine, concat);
    // Make the arg_record to process command-line arguments.
    ArgParser::ArgRecord arg_list[] = {
        {"--model", "The input model file to be processed",
         &cmdLineArgs.modelFilePath, ArgParser::STRING},
        {"--xfig", "Optional output XFig file",
         &cmdLineArgs.xfigFilePath, ArgParser::STRING},        
        {"--xfig-scale", "The size of the output map",
         &cmdLineArgs.figScale, ArgParser::INTEGER},
        {"--search-dist", "Minimum search distance (in miles) to find nodes",
         &cmdLineArgs.minDist, ArgParser::DOUBLE},
        {"--search-scale", "Extra distance/mile to search for path",
         &cmdLineArgs.distScale, ArgParser::DOUBLE},
        {"--best-time", "Return path based on fastest time",
         &cmdLineArgs.useTime, ArgParser::BOOLEAN},
        {"--rnd-test", "Run a given number of random tests",
         &cmdLineArgs.rndTestCount, ArgParser::INTEGER},
        {"--shape", "The input shapefile to be drawn",
         &cmdLineArgs.shapeFilePath, ArgParser::STRING },
        {"--dbf", "The associated DBF file to be used for metadata",
         &cmdLineArgs.dbfFilePath, ArgParser::STRING },
        {"--taxi-rides", "The file with taxi rides to be processed",
         &cmdLineArgs.taxiRidesFile, ArgParser::STRING },
        {"--start-date", "The starting date (mm/dd/yyyy) in taxi rides",
         &cmdLineArgs.startDate, ArgParser::STRING },
        {"--end-date", "The ending date (mm/dd/yyyy) in taxi rides",
         &cmdLineArgs.endDate, ArgParser::STRING },
        {"--start-minute", "The starting minute of the day (multiple of 15, inclusive) in taxi rides (0 to 1440)",
         &cmdLineArgs.startMinute, ArgParser::INTEGER },
        {"--end-minute", "The end minute of the day (multiple of 15, inclusive) in taxi rides (0 to 1440)",
         &cmdLineArgs.endMinute, ArgParser::INTEGER },
        {"--batch-size", "Number of entries to be processed as a batch",
         &cmdLineArgs.batchSize, ArgParser::INTEGER },
        {"--node-summary", "Set file where node summary is printed.",
         &cmdLineArgs.nodeSummaryFile, ArgParser::STRING },
        {"--way-summary", "Set file where way summary is printed.",
         &cmdLineArgs.waySummaryFile, ArgParser::STRING },
        {"", "", NULL, ArgParser::INVALID}
    };
    // Process the command-line arguments.
    ArgParser ap(arg_list);
    ap.parseArguments(argc, argv, true);
    // Ensure at least the shape file is specified.
    if (cmdLineArgs.modelFilePath.empty() ||
        cmdLineArgs.shapeFilePath.empty()) {
        std::cerr << "Specify a model file and shape file to be processed.\n"
                  << ap << std::endl;
        return 1;
    }

    // Things seem fine so far
    return 0;
}

Timestamp
WayRiskAnalyzer::toTimestamp(const std::string& timestamp) const {
    struct tm time;
    const char *eos = timestamp.c_str() + timestamp.size();
    const char *chr = strptime(timestamp.c_str(), "%m/%d/%Y %r", &time);
    // Ensure all the characters have been processed successfully.
    if (chr != eos) {

        std::cerr << "chr = " << (long) chr << ", eos = "
                  << (long) eos << std::endl;
        std::cerr << "Invalid time stamp: " << timestamp << std::endl;
    }
    return time;
}

int WayRiskAnalyzer::run(int argc, char *argv[]) {
    int error = 0;  // Error from various helper methods.
    // First process the command-line args and ensure we have
    // necessary arguments for performing various operations.
    if ((error = processArgs(argc, argv)) != 0) {
        return error;  // Error processing command-line args.
    }
    // Next load the community shape file
    if ((error = osmData.loadModel(cmdLineArgs.modelFilePath)) != 0) {
        return error;  // Error loading community shape file.
    }

    /*
    PathFinder pf(osmData);
    Path path = pf.findBestPath(211318368, 162836650, true,
                                cmdLineArgs.minDist, cmdLineArgs.distScale);
    // Draw the path as xfig
    std::cout << path;    
    pf.generateFig(path, cmdLineArgs.xfigFilePath, cmdLineArgs.figScale);
    */

    // Load census tracts and assign buildings to census tracts, if a
    // census shape file is given.
    if (!cmdLineArgs.shapeFilePath.empty()) {
        shpBldCatalog.buildCatalog(cmdLineArgs.shapeFilePath,
                                   cmdLineArgs.dbfFilePath, osmData);
    }
    
    // Next process taxi cab data if specified.
    if (!cmdLineArgs.taxiRidesFile.empty()) {
        std::ifstream taxiData(cmdLineArgs.taxiRidesFile);
        if (!taxiData.good()) {
            std::cerr << "Error opening " << cmdLineArgs.taxiRidesFile
                      << ". Aborting.\n";
            return 2;
        }
        // Use helper method to process taxi rides data
        processTaxiData(taxiData,
                        toTimestamp(cmdLineArgs.startDate + " 12:00:00 AM"),
                        toTimestamp(cmdLineArgs.endDate   + " 11:59:59 PM"));
    }

    // Print information about all the ways based on visits
    std::ofstream waysData(cmdLineArgs.waySummaryFile);
    if (!waysData.good() && !cmdLineArgs.waySummaryFile.empty()) {
        std::cerr << "Error opening " << cmdLineArgs.waySummaryFile
                  << ". Writing Ways summary to std::cout instead.\n";
    }
    writeWaysSummary(waysData.good() ? waysData : std::cout);

    // Print information about all the nodes based on visits
    std::ofstream nodesData(cmdLineArgs.nodeSummaryFile);
    if (!nodesData.good() && !cmdLineArgs.nodeSummaryFile.empty()) {
        std::cerr << "Error opening " << cmdLineArgs.nodeSummaryFile
                  << ". Writing nodes summary to std::cout instead.\n";
    }
    writeNodeSummary(nodesData.good() ? nodesData : std::cout);
    
    // Everything went well
    return 0;
}

// Get a batch of entries to be processed from taxi rides tsv file
std::vector<WayRiskAnalyzer::RideBatchEntry>
WayRiskAnalyzer::getBatch(std::istream& taxiData, const Timestamp& startDay,
                          const Timestamp& endDay, const int startMinute,
                          const int endMinute, const size_t batchSize,
                          const int stAreaCol, const int endAreaCol) {
    std::vector<RideBatchEntry> batch;
    for (std::string line; (batch.size() < batchSize) &&
             std::getline(taxiData, line);) {
        // If the current entry is not within our time window, ignore it.
        const std::string startTime = getColumn(line, 2);
        const std::string endTime   = getColumn(line, 2);
        const std::string stAreaID  = getColumn(line, stAreaCol);
        const std::string endAreaID = getColumn(line, endAreaCol);

        if (startTime.empty() || endTime.empty() || stAreaID.empty() ||
            endAreaID.empty()) {
            // std::cerr << "Invalid line: startTime: " << startTime
            //           << ", endTime: " << endTime << ", startAreaID: "
            //           << stAreaID << ", endAreaID: " << endAreaID
            //           << ", TripID: " << getColumn(line, 0) << std::endl;
            continue;
        }
        const Timestamp startTS = toTimestamp(startTime);
        const Timestamp endTS   = toTimestamp(endTime);
        if (isHigher(startDay, startTS) && isHigher(endTS, endDay) &&
            (startMinute <= minuteOfDay(startTS)) &&
            (endMinute   >= minuteOfDay(endTS))) {
            // Found a matching entry
            const std::string distStr = getColumn(line, 5);
            if (distStr.empty()) {
                std::cerr << "Invalid distance: distStr: " << distStr
                          << ", TripID: " << getColumn(line, 0) << std::endl;
                continue;
            }
            const double dist  = std::stod(distStr);
            // Add an entry for this ride
            batch.push_back({stAreaID, endAreaID, startTS, endTS, dist});
        }
    }
    ASSERT (batch.size() <= batchSize);
    return batch;
}

void
WayRiskAnalyzer::processTaxiData(std::istream& taxiData,
                                 const Timestamp startDay,
                                 const Timestamp endDay) {
    std::string line;
    std::getline(taxiData, line);  // read & ignore header
    // Compute the maximum number of entries for each node vector
    const int maxEntries = (cmdLineArgs.endMinute -
                            cmdLineArgs.startMinute) / 15 + 1;
    // Process data in batches to streamline I/O, memory, and
    // multithreading overheads.
    int pathCount = 0;
    Stopwatch timer;  // Just for informational/performance tuning.
    do {
        timer.start();
        const std::vector<RideBatchEntry> batch =
            getBatch(taxiData, startDay, endDay, cmdLineArgs.startMinute,
                     cmdLineArgs.endMinute, cmdLineArgs.batchSize);
        std::cout << "Read next batch in " << timer.elapsed().count() / 1e3
                  << " seconds.\n";
        // Process the batch and report timing
        timer.start();
#pragma omp parallel
        {
            // Node visits are first tracked in a thread-local map and
            // then added to the overall nodeVisits map at the end of
            // the parallel block.
            NodeVisitMap localNodeVisits;
            WayVisitMap  localWayVisits;
#pragma omp for
            for (size_t i = 0; (i < batch.size()); i++) {
                const RideBatchEntry& rideInfo = batch[i];
                const int stBldId = shpBldCatalog.getRndBldID(rideInfo.startRegion);
                const int enBldId = shpBldCatalog.getRndBldID(rideInfo.endRegion);
                if ((stBldId == -1) || (enBldId == -1)) {
                    std::cerr << "Did not find building for: start = "
                              << rideInfo.startRegion << ", stBldId = "
                              << stBldId << ", end = " << rideInfo.endRegion
                              << ", enBldId = " << enBldId << std::endl;
                    continue;  // This ring did not have any buildings?
                }
                // Find a pathway between the two biuldings
                PathFinder pf(osmData);
                Path path = pf.findBestPath(stBldId, enBldId, true,
                                            cmdLineArgs.minDist,
                                            cmdLineArgs.distScale);
                // Update the nodes visited in the path
                updateNodes(path, localNodeVisits, localWayVisits,
                            minuteOfDay(rideInfo.startTime) -
                            cmdLineArgs.startMinute, maxEntries);
            } // OMP for

#pragma omp critical (processTaxiData_nodes)
            {
                // Copy over all the node visits
                for (const auto& entry : localNodeVisits) {
                    auto& destVec = nodeVisits[entry.first];
                    destVec.resize(maxEntries);
                    for (int i = 0; (i < maxEntries); i++) {
                        destVec[i] += entry.second[i];
                    }
                }
            }

#pragma omp critical (processTaxiData_ways)
            {
                // Copy over all the node visits
                for (const auto& entry : localWayVisits) {
                    wayVisits[entry.first] += entry.second;
                }
            }
            
            
        }  // OMP-parallel

        // Track time taken to process this batch.
        const auto elapsedTime = timer.elapsed();
        pathCount += batch.size();        
        std::cout << "Completed batch processing in  " 
                  << elapsedTime.count() / 1e3 << " seconds. Total paths: "
                  << pathCount << std::endl;
        // Break out of the loop if all lines have been processed.
        if (batch.size() < (size_t) cmdLineArgs.batchSize) {
            break;  // Done processing the whole file.
        }
    } while (true);
}

void
WayRiskAnalyzer::updateNodes(const Path& path, NodeVisitMap& nodeVisits,
                             WayVisitMap& wayVisits,
                             const int startMinute, const int maxEntries) {
    for (const PathSegment& seg : path) {
        // Update the visit count for the 'ways'
        wayVisits[seg.wayID]++;
        if (seg.buildingID == -1) {
            // Ensure we have sufficient vector entires in this node.
            nodeVisits[seg.nodeID].resize(maxEntries);
            // Compute the time corresponding to this path
            const double segTime   = (startMinute + seg.distance * 60);
            ASSERT(segTime >= 0);
            const int timeIndex = std::round(segTime / 15);
            if (timeIndex < maxEntries) {
                nodeVisits[seg.nodeID][timeIndex]++;
            }
        }
    }
}

// Simple helper method to convert minutes to 24-hour format
std::string
WayRiskAnalyzer::minToStr(const int min) const {
    const int hours = min / 60, mins = min % 60;
    return std::to_string(hours) + ":" + std::to_string(mins);
}

void
WayRiskAnalyzer::writeNodeSummary(std::ostream& os) const {
    // First print the fixed part of the header for the file.
    os << "# Node Summary\n"
       << "# Generated using command-line: " << cmdLineArgs.fullCmdLine
       << "\nNodeID\tOSMid\tLatitude\tLongitude\t#Visits";
    // Print the times for which we are printing summary info.
    for (int min = cmdLineArgs.startMinute; (min <= cmdLineArgs.endMinute);
         min += 15) {
        os << '\t' << minToStr(min);
    }
    os << std::endl;

    // Now that we have printed the header, it is time to print node
    // information.
    for (const auto& entry : nodeVisits) {
        const auto& visits = entry.second;
        const Node& node   = osmData.nodeList.at(entry.first);
        // Print information about the node.
        os << entry.first    << node.osmId << '\t' << node.latitude << '\t'
           << node.longitude << '\t' << sum(visits);
        for (const auto count : visits) {
            os << '\t' << count;
        }
        os << '\n';
    }
}

// Write summary of ways
void
WayRiskAnalyzer::writeWaysSummary(std::ostream& os) const {
    // First print the fixed part of the header for the file.
    os << "# Ways Summary\n"
       << "# Generated using command-line: " << cmdLineArgs.fullCmdLine
       << "\nOSM_WayID\tName\tKind\tSpeed\tOneWay\tDeadEnd\t#Nodes\t"
       << "#VistedNodes\t#Visits";
    // Print the times for which we are printing summary info.
    for (int min = cmdLineArgs.startMinute; (min <= cmdLineArgs.endMinute);
         min += 15) {
        os << '\t' << minToStr(min);
    }
    os << "\tNodeInfo (nodeID,OSMid,lat,lon)...\n";

    // Print information for each way in the wayVisits map
    for (const auto& entry : wayVisits) {
        // Get the way from the model to print its information.
        const Way way = osmData.wayMap.at(entry.first);
        // Print the basic information about the way first.
        os << way.id << '\t' << way.name << '\t' << way.getKindStr() << '\t'
           << way.maxSpeed << '\t' << way.isOneWay << '\t' << way.isDeadEnd
           << '\t' << way.nodeList.size();

        // Next we have to collate the visits for each node on this
        // 'way', at different times.
        std::vector<int> visits;
        int visitedNodes = 0;
        for (const auto nodeID : way.nodeList) {
            if (nodeVisits.find(nodeID) != nodeVisits.end()) {
                visitedNodes++;
                const std::vector<int>& src = nodeVisits.at(nodeID);
                visits.resize(src.size());
                // For each time step, accummulate the values
                for (size_t i = 0; (i < src.size()); i++) {
                    visits[i] += src[i];
                }
            }
        }

        // Print the number of visted nodes
        os << '\t' << visitedNodes;
        // Print the total visits for this node
        os << '\t' << sum(visits);
        // Print the visits for this way at different times.
        for (const auto count : visits) {
            os << '\t' << count;
        }
        
        // Finally print information on all the nodes in the way for
        // cross referencing
        os << '\t';
        for (const auto nodeID : way.nodeList) {
            const Node& node = osmData.nodeList.at(nodeID);
            os << ' ' << nodeID << ',' << node.osmId << ","
               << node.latitude << ',' << node.longitude;
        }

        // Also print all the visits for each node for corss reference
        for (const auto nodeID : way.nodeList) {
            os << '\t';
            if (nodeVisits.find(nodeID) != nodeVisits.end()) {
                os << sum(nodeVisits.at(nodeID));
            } else {
                os << '0';
            }
        }
        
        os << '\n';
    }
}

/**
 * A simple top-level main method that delegates the work off to the
 * WayRiskAnalyzer::run method to do all the necessary operations.
 */
int main(int argc, char *argv[]) {
    WayRiskAnalyzer analyzer;
    return analyzer.run(argc, argv);
}

#endif
