#ifndef ACCIDENTS_COLLATOR_CPP
#define ACCIDENTS_COLLATOR_CPP

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

#include <climits>
#include <numeric>
#include <sstream>
#include "Utilities.h"
#include "OSMData.h"
#include "ShapeFile.h"
#include "AccidentsCollator.h"

int
AccidentsCollator::processArgs(int argc, char *argv[]) {
    // Save the command-line arguments for future reference.
    auto concat = [](std::string s1, std::string s2) { return s1 + " " + s2; };
    fullCmdLine = std::accumulate(argv, argv + argc, fullCmdLine, concat);
    // Make the arg_record to process command-line arguments.
    ArgParser::ArgRecord arg_list[] = {
        {"--model", "The input model file to be processed",
         &cmdLineArgs.modelFilePath, ArgParser::STRING},
        {"--xfig", "Optional output XFig file",
         &cmdLineArgs.xfigFilePath, ArgParser::STRING},        
        {"--xfig-scale", "The size of the output map",
         &cmdLineArgs.figScale, ArgParser::INTEGER},
        {"--xfig-x-clip", "Value to subtract to left-justify figure",
         &cmdLineArgs.xClip, ArgParser::INTEGER},
        {"--xfig-y-clip", "Value to subtract to top-justify figure",
         &cmdLineArgs.yClip, ArgParser::INTEGER},
        {"--max-dist", "Maximum distance (in miles) from accident to nodes",
         &cmdLineArgs.maxDist, ArgParser::DOUBLE},
        {"--accidents-tsv", "The input accidents TSV file to be processed",
         &cmdLineArgs.accidentsFile, ArgParser::STRING },        
        {"--output-tsv", "The output TSV file with node data",
         &cmdLineArgs.outputFile, ArgParser::STRING },
        {"--start-date", "The starting date (mm/dd/yyyy) in taxi rides",
         &cmdLineArgs.startDate, ArgParser::STRING },
        {"--end-date", "The ending date (mm/dd/yyyy) in taxi rides",
         &cmdLineArgs.endDate, ArgParser::STRING },
        {"--batch-size", "Number of lines processed as a big batch",
         &cmdLineArgs.batchSize, ArgParser::INTEGER },
        {"--shape", "The input shapefile to be drawn",
         &cmdLineArgs.shapeFilePath, ArgParser::STRING },
        {"--dbf", "The associated DBF file to be used for metadata",
         &cmdLineArgs.dbfFilePath, ArgParser::STRING },
        {"", "", NULL, ArgParser::INVALID}
    };
    // Process the command-line arguments.
    ArgParser ap(arg_list);
    ap.parseArguments(argc, argv, true);
    // Ensure at least the shape file is specified.
    if (cmdLineArgs.modelFilePath.empty() ||
        cmdLineArgs.accidentsFile.empty() || cmdLineArgs.outputFile.empty()) {
        std::cerr << "Must specify the following 3 options:\n"
                  << "--model, --accidents-tsv, --output\n"
                  << ap << std::endl;
        return 1;
    }

    // Things seem fine so far
    return 0;
}

Timestamp
AccidentsCollator::toTimestamp(const std::string& timestamp) const {
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

int AccidentsCollator::run(int argc, char *argv[]) {
    int error = 0;  // Error from various helper methods.
    // First process the command-line args and ensure we have
    // necessary arguments for performing various operations.
    if ((error = processArgs(argc, argv)) != 0) {
        return error;  // Error processing command-line args.
    }

    // Ensure that the traffic accidents file is readable.
    std::ifstream accidentsData(cmdLineArgs.accidentsFile);
    if (!accidentsData.good()) {
        std::cerr << "Unable to read accidents file "
                  << cmdLineArgs.accidentsFile << std::endl;
    }

    // Next load the community shape file
    OSMData osmData;  // model with ways and nodes
    if ((error = osmData.loadModel(cmdLineArgs.modelFilePath)) != 0) {
        return error;  // Error loading community shape file.
    }

    // Ensure that the output file is writable
    std::ofstream outputData(cmdLineArgs.outputFile);
    if (!outputData.good()) {
        std::cerr << "Unable to open output file for reading: "
                  << cmdLineArgs.outputFile << std::endl;
    }

    // Use a helper method to do the actual processing
    const AccidentsMap accidents = collate(accidentsData, osmData, outputData);

    // Use helper method to write the data to given output file.
    write(accidents, osmData, outputData);

    // If an xfig file has been specified, then write xfig output
    if (!cmdLineArgs.xfigFilePath.empty()) {
        genXFig(accidents, osmData);
    }
    
    // Everything went well
    return 0;
}

double
AccidentsCollator::logScaleAcc(const double acc, const double minAcc,
                               const double maxAcc) const {
    double accRatio    = (acc - minAcc) / (maxAcc - minAcc);
    const double range = log10(maxAcc / minAcc);
    if (range > 1) {
        accRatio = (range + log10(accRatio)) * 100.0 / range;
        accRatio = std::max(0.0, std::min(100.0, accRatio));
        
    } else {
        accRatio *= 100.0;
    }
    return accRatio;
}

void AccidentsCollator::genXFig(const AccidentsMap& accidents,
                                const OSMData& model) {
    XFigHelper xfig;
    if (!xfig.setOutput(cmdLineArgs.xfigFilePath, true)) {
        std::cerr << "Error writing xfig file " << cmdLineArgs.xfigFilePath
                  << std::endl;
        return;
    }
    // Add some comments to the XFig file on how this file was generated.
    xfig.addComment("Accidents collated information on " + getSystemTime());
    xfig.addComment("Commands run as: " + fullCmdLine);

    // First draw any shapes that is specified.
    int xClip = -1, yClip = -1;
    if (!cmdLineArgs.shapeFilePath.empty()) {
        ShapeFile shpFile;
        if (!shpFile.loadShapes(cmdLineArgs.shapeFilePath,
                                cmdLineArgs.dbfFilePath)) {
            std::cerr << "Error loading shape/DBF file: "
                      << cmdLineArgs.shapeFilePath << " or "
                      << cmdLineArgs.dbfFilePath << std::endl;
            return;
        }
        shpFile.genXFig(xfig, xClip, yClip, cmdLineArgs.figScale, false, {},
                        false,cmdLineArgs.xfigFilePath);
    }
    
    // To draw a nice figure with log-scale based coloring of
    // accidents, we need to compute bounds, which is done in the loop below.
    int minAcc = INT_MAX, maxAcc = 0;
    double minLat = 90, minLon = 180;
    for (const auto& entry : accidents) {
        long nodeID = entry.first;   // Convenience
        const AccidentInfo& info = entry.second;
        // Get the node from the model to get it's latitude and longitude.
        const Node& node = model.nodeList.at(nodeID);
        // Track the accident-count bounds
        minAcc = std::min(minAcc, info.count);
        maxAcc = std::max(maxAcc, info.count);
        // Track lat lon for bounds
        minLat = std::min(minLat, node.latitude);
        minLon = std::min(minLon, node.longitude);
    }

    std::cout << "minAcc = " << minAcc << ", maxAcc = " << maxAcc << std::endl;
    
    // Update the clipping bounds, if xClip or yClip is not set?
        
    // Now add points for each node in our list
    for (const auto& entry : accidents) {
        long nodeID = entry.first;   // Convenience
        const AccidentInfo& info = entry.second;
        // Get the node from the model to get it's latitude and longitude.
        const Node& node = model.nodeList.at(nodeID);
        // Create a comment information about the node
        std::ostringstream os;
        os << nodeID         << '\t' << node.latitude << '\t'
           << node.longitude << '\t'
           << info.count  << '\t' << info.injuries << '\t'
           << info.deaths << '\t' << info.info;
        xfig.addComment(os.str());
        // Next, get the XFig coordinates for adding a visible marker.
        int xfigX, xfigY;
        getXYValues(node.latitude, node.longitude, xfigX, xfigY,
                    cmdLineArgs.figScale * XFIG_SCALE, false);
        // Account for clipping bounds
        xfigX -= (xClip != -1 ? xClip : cmdLineArgs.xClip + xClip);
        xfigY -= (yClip != -1 ? yClip : cmdLineArgs.yClip);
        // Now actually add the visible marker.
        const int fillColor = 32 + logScaleAcc(info.count, minAcc, maxAcc);
        // Setup layers based on the number of accidents to ease visualization
        const int layer = 70 + info.count * 2;
        // std::cout << "fillColor = " << fillColor << std::endl;
        xfig.drawRect(xfigX - 120, xfigY - 60, 240, 120, fillColor, layer + 1);
        xfig.drawText(std::to_string(info.count), xfigX, xfigY - 60,
                      CENTER_JUSTIFIED, 14, 8, BLACK, layer);
    }

    // Generate tics for the scale bar.
    std::vector<double> tics = {0, (double) minAcc};
    double prevPerc = 0;    
    for (int frac = 512; (frac > 2); frac /= 4) {
        const double acc = minAcc + (maxAcc - minAcc) / frac;
        const double perc = std::trunc(logScaleAcc(acc, minAcc, maxAcc));
        if (perc > prevPerc) {
            tics.push_back(perc);
            tics.push_back(acc);
            prevPerc = perc;
        }
    }
    tics.push_back(100);
    tics.push_back(maxAcc);
    // The scale bar is drawn proportional to the size of the map
    // so that it is readable/visible.  For this we have to play a
    // bit with scale to make this visually appealing.
    xfig.drawScaleBar(XFIG_SCALE, cmdLineArgs.figScale / 100, 800 * XFIG_SCALE,
                      50 * XFIG_SCALE, tics, 16);
}

void AccidentsCollator::write(const AccidentsMap& accidents,
                              const OSMData& model, std::ostream& os) {
    // First write the fixed header lines
    os << "# Accidents collated information on " << getSystemTime() << '\n'
       << "# Command run as: " << fullCmdLine    << '\n'
       << "# NodeID\tOSMId\tLatitude\tLongitude\tAccCount\tInjuries\t"
       << "Deaths\tOtherInfo\n";

    // Write information about each node for which we found an accident
    for (const auto& entry : accidents) {
        long nodeID = entry.first;   // Convenience
        const AccidentInfo info = entry.second;
        // Get the node from the model to get it's latitude and longitude.
        const Node& node = model.nodeList.at(nodeID);
        // Print information about the accident
        os << nodeID         << '\t' << node.osmId << '\t'
           << node.latitude  << '\t'
           << node.longitude << '\t'
           << info.count  << '\t' << info.injuries << '\t'
           << info.deaths << '\t' << info.info  << '\n';
    }
}

std::vector<std::string>
AccidentsCollator::getNextBatch(std::istream& accidentsData, int& lNum,
                                const int batchSize) const {
    std::vector<std::string> batch;
    std::string line;
    for (int i = 0; (i < batchSize) && std::getline(accidentsData, line); i++) {
        batch.push_back(line);
        lNum++;
    }
    return batch;
}


AccidentsCollator::AccidentsMap
AccidentsCollator::collate(std::istream& accidentsData,
                           const OSMData& model, std::ostream& out) {
    // Create a map of nodes and traffic accidents that happen there
    AccidentsMap accidents;
    // Setup timestamp for filtering out accidents
    const Timestamp startDate = toTimestamp(cmdLineArgs.startDate +
                                            " 12:00:00 AM");
    const Timestamp endDate   = toTimestamp(cmdLineArgs.endDate   +
                                            " 11:59:59 PM");
    // Read line-by-line from the accidentsData and process it. The
    // first line is a header which we read and ignore.
    std::string line;
    std::getline(accidentsData, line);  // ignore the header
    int lNum = 1;
    
    // Process batches of lines at a time to ease multithreading
    do {
        const auto batch = getNextBatch(accidentsData, lNum,
                                        cmdLineArgs.batchSize);
        if (batch.empty()) {
            break;  // Done processing all the lines in the data file.
        }

#pragma omp parallel for
        for (size_t i = 0; (i < batch.size()); i++) {
            const std::string line = batch[i];
            // Filter out accidents that don't fall within the specified
            // date ranges.
            const Timestamp accTime = toTimestamp(getColumn(line, 3));
            if (!(isHigher(startDate, accTime) && isHigher(accTime, endDate))) {
                continue;  // Accident not within specified date range.
            }
            
            // Get the latitude and longitude for the accident.
            const auto latStr= getColumn(line, 46);
            const auto lonStr= getColumn(line, 47);
            // Some traffic accidents don't have latitude and longitude
            // filled-in.  We ignore these for now.
            if (latStr.empty() || lonStr.empty()) {
#pragma omp critical (cerr)
                std::cerr << "Accident entry on line #" << lNum
                          << " did not have "
                          << "latitude or longitude. Ignoring line.\n";
                continue;
            }
            const double lat = std::stod(latStr);
            const double lon = std::stod(lonStr);
            // Find the nearest node for the accident location.
            long nearNodeID = -1;      // ID of the nearest node        
            double nearNodeDist = -1;  // Distance to nearest node
            for (size_t nodeID = 0; (nodeID < model.nodeList.size());
                 nodeID++) {
                const Node& node = model.nodeList[nodeID];
                const double currDist =
                    getDistance(node.latitude, node.longitude, lat, lon);
                if ((nearNodeID == -1) || (currDist < nearNodeDist)) {
                    // Found a node that is closer.
                    nearNodeID   = nodeID;
                    nearNodeDist = currDist;
                }
            }
            // If we found a node or if the node is too far, then we
            // just report a warning and move on to the next accident.
            if ((nearNodeID == -1) || (nearNodeDist > cmdLineArgs.maxDist)) {
#pragma omp critical (cerr)
                std::cerr << "Unable to find node for accident on line #"
                          << (lNum - i) << " with lat: " << lat << ", lon: "
                          << lon << ". Nearest node was: " << nearNodeID
                          << ", distance: "  << nearNodeDist << " miles.\n";
                continue;
            }
            // Ok, we got a node for this accident. Let's collate the data.
            const int injuries = std::stoi(getColumn(line, 36, "0"));
            const int deaths   = std::stoi(getColumn(line, 37, "0"));
#pragma omp critical (update)
            {
                auto& entry  = accidents[nearNodeID];
                entry.count++;   // Number of accidents
                entry.injuries += injuries;
                entry.deaths   += deaths;
            }
        }
        std::cout << "Done processing " << lNum << " lines.\n";
    } while (true);  // Batch-processsing loop
    // Return the collated accidents
    return accidents;
}

/**
 * A simple top-level main method that delegates the work off to the
 * AccidentsCollator::run method to do all the necessary operations.
 */
int main(int argc, char *argv[]) {
    AccidentsCollator collator;
    return collator.run(argc, argv);
}

#endif
