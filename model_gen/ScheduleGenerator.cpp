#ifndef SCHEDULE_GENERATOR_CPP
#define SCHEDULE_GENERATOR_CPP

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
#include <climits>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <utility>
#include "Utilities.h"
#include "ScheduleGenerator.h"
#include "ShapeFile.h"
#include "OSMData.h"
#include "PathFinder.h"
#include "WorkBuildingSelector.h"
#include "Stopwatch.h"

int
ScheduleGenerator::run(int argc, char *argv[]) {
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

    // Next load the model from the specified model file.
    OSMData model;
    if ((error = model.loadModel(cmdLineArgs.modelFilePath)) != 0) {
        std::cerr << "Error loading model from: " << cmdLineArgs.modelFilePath
                  << std::endl;
        return error;
    }

    // Generate schedules for every person in the home buildings in
    // the model
    XFigHelper xfig;    
    generateSchedule(model, xfig, cmdLineArgs.key_info);
    
    // Next open an XFig file for drawing all of the necesssary
    // information.
    if (!cmdLineArgs.xfigFilePath.empty()) {
        if ((error = drawXfig(xfig)) != 0) {
            std::cerr << "Error writing figure to: " << cmdLineArgs.xfigFilePath
                      << std::endl;
            return error;
        }
    }

    return 0;  // All went well.
}

int
ScheduleGenerator::drawXfig(XFigHelper& xfig) {
    if (!xfig.setOutput(cmdLineArgs.xfigFilePath, true)) {
        std::cerr << "Error writing to fig file: "
                  << cmdLineArgs.xfigFilePath << std::endl;
        return 10;
    }
    
    // Next load and draw the shape files
    int xClip = -1, yClip = -1, error = 0;
    if ((error = drawShapeFiles(xfig, cmdLineArgs.shapeFilePaths,
                                cmdLineArgs.dbfFilePaths,
                                xClip, yClip,
                                cmdLineArgs.figScale,
                                cmdLineArgs.startShapeLevel)) != 0) {
        return error;  // Error loading community shape file.
    }
    
    return 0;  // All went well.
}

std::tuple<BuildingMap, BuildingMap, BuildingList>
ScheduleGenerator::getHomeAndNonHomeBuildings(const BuildingMap& buildingMap) const {
    BuildingMap homeBuildings, nonHomeBuildings;
    BuildingList nonHomeBldList;
    for (auto item = buildingMap.begin(); item != buildingMap.end(); item++) {
        if (item->second.isHome) {
            homeBuildings[item->first] = item->second;
        } else {
            Building bld = item->second;
            // Initialize capacity of this office building based on
            // its square footage.
            bld.population = bld.getArea() / cmdLineArgs.offSqFtPer;
            nonHomeBuildings[bld.id] = bld;
            nonHomeBldList.push_back(bld);
        }
    }
    
    ASSERT(homeBuildings.size() + nonHomeBuildings.size() ==
           buildingMap.size());
    
    std::cout << "# of non-home buildings: " << nonHomeBuildings.size() << '\n';
    std::cout << "# of home buildings: " << homeBuildings.size() << std::endl;

    return {homeBuildings, nonHomeBuildings, nonHomeBldList};
}

// Given a building, find the longest travel-to-work time among all
// people whose means of transport is 1
std::pair<int, int>
ScheduleGenerator::findLongestShortestToWorkTime(const Building& bld,
                                                 int travelTimeIdx,
                                                 int meansOfTransportationIdx) const {
    int maxTime = -1;
    int minTime = INT_MAX;

    for (size_t i = 0; i < bld.households.size(); i++) {
        const auto& householdPeopleInfo = bld.households.at(i).getPeopleInfo();
        for (size_t j = 0; j < householdPeopleInfo.size(); j++) {
            const auto& curPerson = householdPeopleInfo.at(j);

            int travelTimeToWork = -1, transportMeans = -1;

            try {
                travelTimeToWork = curPerson.getIntegerInfo(travelTimeIdx);
                transportMeans = curPerson.getIntegerInfo(meansOfTransportationIdx);
            } catch (const std::out_of_range& e) {
                std::cerr << "Skipping person with ID: "
                          << curPerson.getPerID()
                          << " due to insufficient amount of data\n";
                continue;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Skipping person with ID: "
                          << curPerson.getPerID()
                          << " because either travel time to work or "
                          << "transportation means is not an integer\n";
                continue;
            }

            // We only care about transportMeans == 1 with a
            // valid work travel time
            if (transportMeans != 1 || travelTimeToWork <= 0) {
                continue;
            }

            if (travelTimeToWork > maxTime) {
                maxTime = travelTimeToWork;
            }
            
            if (travelTimeToWork < minTime) {
                minTime = travelTimeToWork;
            }
        }
    }
    return {minTime, maxTime};
}


// Return a list of potential non-home buildings to where people may
// travel between the specified minTravelTime - timeMargin and
// maxTravelTime + timeMargin.
BuildingList
ScheduleGenerator::getCandidateWorkBuildings(const Building& srcBld,
                                             const BuildingMap& nonHomeBlds,
                                             const BuildingList& nonHomeBldsList,
                                             const int minTravelTime,
                                             const int maxTravelTime,
                                             const int timeMargin) const {
    // There are some work travel times that are pretty large which do
    // not approximate well.  In this situation, we track the building
    // with maximum distance and use that building as the fall back.
    const Building* maxDistBld = &srcBld;
    double maxDist = 0;
    
    // The list of potential candidates.
    BuildingList candidateBlds;

#pragma omp parallel for schedule(guided)
    for (size_t i = 0; (i < nonHomeBldsList.size()); i++) {
        const auto& bld  = nonHomeBldsList.at(i);
        // const auto bldId = bld.id;
        // for (const  auto& [bldId, bld] : nonHomeBlds) {
        // Get the distance in miles from the source building to an
        // non-office building.
        const double dist = getDistance(srcBld.wayLat, srcBld.wayLon,
                                        bld.wayLat,    bld.wayLon);
        // Track maximum distance building
        if (dist > maxDist) {
            maxDistBld = &nonHomeBldsList.at(i);
            maxDist    = dist;
        }
        // Convert distance to time using an "average" estimated time.
        // This approach is used because computing actual path is
        // slower.
        const int timeInMinutes = std::round(dist * 60 / cmdLineArgs.avgSpeed);

        // Use only buildings that are within our specified time ranges
        if ((timeInMinutes >= (minTravelTime - timeMargin)) &&
            (timeInMinutes <= (maxTravelTime + timeMargin)) &&
            (bld.population > 0)) {
            // Multi-threading issue.
#pragma omp critical
            candidateBlds.push_back(bld);
        }
    }

    // Sort the candidate non-home buildings based on their distances
    // to current building (approximating travel time)
    // std::sort(candidateBlds.begin(), candidateBlds.end(),
    //           [&srcBld](const Building& b1, const Building& b2) {
    //               return getDistance(srcBld.wayLat, srcBld.wayLon,
    //                                  b1.wayLat, b1.wayLon) > 
    //                   getDistance(srcBld.wayLat, srcBld.wayLon,
    //                               b2.wayLat, b2.wayLon);
    //           });

    // std::cout << "# of candidate work locations: " << candidateBlds.size()
    //           << std::endl;
    if (candidateBlds.empty()) {
        std::cout << "Unable to find candidate buildings for time: "
                  << minTravelTime << " for building: " << srcBld.id
                  << ". Using max distance building: " << maxDistBld->id
                  << ", at a distance of: " << maxDist << std::endl;
        candidateBlds.push_back(*maxDistBld);
        /*
        for (const  auto& [bldId, bld] : nonHomeBlds) {
            // Get the distance in miles from the source building to an
            // non-office building.
            const double dist = getDistance(srcBld.wayLat, srcBld.wayLon,
                                            bld.wayLat,    bld.wayLon);
            // Convert distance to time using an "average" estimated time.
            // This approach is used because computing actual path is
            // slower.
            const int timeInMinutes =
                std::round(dist * 60 / cmdLineArgs.avgSpeed);
            std::cout << "    Building: " << bldId << ", dist: " << dist
                      << ", time: " << timeInMinutes << ".\n";
        }
        */
    }
    return candidateBlds;
}

TrvlTimePeopleMap
ScheduleGenerator::getTimePeopleMap(const Building& bld) const {
    std::unordered_map<int, std::vector<long>> timePeopleMap;
    for (size_t i = 0; i < bld.households.size(); i++) {
        const auto& curHousehold = bld.households.at(i);
        for (const auto& curPer : curHousehold.getPeopleInfo()) {
            int travelTimeToWork = -1, transportMeans = -1;
            try {
                travelTimeToWork = curPer.getIntegerInfo(cmdLineArgs.jwmnpIdx);
                transportMeans = curPer.getIntegerInfo(cmdLineArgs.jwtrnsIdx);
            } catch (const std::out_of_range& e) {
                std::cerr << "Skipping person with ID: "
                          << curPer.getPerID()
                          << " due to insufficient amount of data\n";
                continue;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Skipping person with ID: " << curPer.getPerID()
                          << " because either travel time to work or"
                          << " transportation means is not an integer\n";
                continue;
            }
            
            if (transportMeans != 1 || travelTimeToWork <= 0) {
                continue;
            }

            timePeopleMap[travelTimeToWork].push_back(curPer.getPerID());
        }
    }

    // Print the map for debugging
    /*
    for (auto& it : timePeopleMap) {
        std::cout << it.first << ": " << it.second.size() << " people [ ";
        for (long l : it.second) {
            std::cout << l << " ";
        }
        std::cout << "]" << std::endl;
    }
    */

    return timePeopleMap;
}

/* The genral algorithm implemented within the method below:
    Goal: we are assigning people working buildings 
    based on their travel-to-work time in the model

    Iterate over all home buildings, and for each building `bld`:
        1. Find the minimum and maximum travel-to-work time among
        all the people living in `bld` using the
        `findLongestShortestToWorkTime` method

        2. Initialize the list of all potential office buildings that
        can be assigned to people within `bld` with all non-home 
        buildings to begin with
        (pseudo-code: possibleWorkBuildingsForCurBuilding <-- nonHomeBuildings)

        3. Sort vector `possibleWorkBuildingsForCurBuilding` by travel time
        from `bld` to the building in the list

        4. Create a time-person map with the key being travel-to-work time
        and the value being the ID of the people within `bld` that has the
        same time in the key

        5. For every building in `possibleWorkBuildingsForCurBuilding` vector
        (let's call it `pbld`), find the time from `bld` to `pbld` and
        see whether there is anyone from the map who fits that time. If
        so, assign `pbld` to the last person in the map with the time
        and pop both the building from the vector and the person from the map.
        Repeat this until everybody gets an assigned building. If not, we add
        an margin to the time to see if a fuzzy match can be found
        (This assignment process is likely the performance bottleneck)
*/

/*
void
ScheduleGenerator::generateSchedule(const OSMData& model, XFigHelper& fig,
                                    const std::string& infoKey) {
    std::cout << "Generating Schedule..." << std::endl;

    // Classify buildings by their types in order to reduce the number
    // of buildings we iterate on for different operations.
    auto [homeBuildings, nonHomeBuildings] =
        getHomeAndNonHomeBuildings(model.buildingMap);
    
    for (auto& [bldId, bld] : homeBuildings) {
        if (bld.households.empty()) {
            continue;  // no households in this home
        }
        std::cout << "Current Building ID: " << bld.id << std::endl;
        const auto [minTravelTime, maxTravelTime] = 
            findLongestShortestToWorkTime(bld, cmdLineArgs.jwmnpIdx,
                                          cmdLineArgs.jwtrnsIdx);
        if (maxTravelTime < 0) {
            // If the travel time is negative, it means there is
            // nothing to do for this building.
            continue;
        }

        const int timeMargin = 3;  // Wiggle room of 3 minutes
        BuildingList candidateWorkBlds =
            getCandidateWorkBuildings(bld, nonHomeBuildings, minTravelTime,
                                      maxTravelTime, 0);
        if (!candidateWorkBlds.empty()) {
            // Get all people info of the current building
            TrvlTimePeopleMap timePeopleMap = getTimePeopleMap(bld);
            std::unordered_map<long, long> workBuildingAssignment =
                assignWorkBuildings(model, bld, nonHomeBuildings,
                                    candidateWorkBlds,
                                    timePeopleMap, timeMargin);
        } else {
            std::cerr << "Unable to find office building for people in "
                      << "building id #" << bldId << std::endl;
        }
    }
}
*/

void
ScheduleGenerator::generateSchedule(const OSMData& model, XFigHelper& fig,
                                    const std::string& infoKey) {
    std::cout << "Generating Schedule..." << std::endl;
    // Create the building selector heler and initialize the object.
    // WorkBuildingSelector wbs(model);
    // wbs.genOrUseTrvlEst(cmdLineArgs.numBldPairs, cmdLineArgs.useTrvlEstFile,
    //                     cmdLineArgs.outTrvlEstFile);
    // Temporarily returning from here to assess travel estimation timings
    // return;
 
    // Classify buildings by their types in order to reduce the number
    // of buildings we iterate on for different operations.
    auto [homeBuildings, nonHomeBuildings, nonHomeBldList] =
        getHomeAndNonHomeBuildings(model.buildingMap);
    Stopwatch timer;
    for (auto& [bldId, bld] : homeBuildings) {
        timer.start();
        if (bld.households.empty()) {
            continue;  // no households in this home
        }
        // For each person in each household assign work building
        for (auto& hld : bld.households) {
            for (auto& ppl : hld.getPeopleInfo()) {
                if (ppl.getIntegerInfo(cmdLineArgs.jwtrnsIdx) != 1) {
                    continue;  // This person doesn't drive to work.
                }
                const int travelTime = ppl.getIntegerInfo(cmdLineArgs.jwmnpIdx);
                if (travelTime < 0) {
                    continue;  // This person doesn't travel for work
                }
                // Find buildings in the
                const int timeMargin = 3;  // Wiggle room of 3 minutes
                for (int slack = 0; slack < timeMargin; slack++) {
                    BuildingList candidateWorkBlds =
                        getCandidateWorkBuildings(bld, nonHomeBuildings,
                                                  nonHomeBldList,
                                                  travelTime, travelTime,
                                                  slack);
                    if (!candidateWorkBlds.empty()) {
                        assignWorkBuildings(model, bld, nonHomeBuildings,
                                            candidateWorkBlds,
                                            ppl, timeMargin);
                    }
                }
            } // for each household
        }  // for each building
        std::cout << "Processing time for building " << bldId
                  << " with " << bld.households.size() << " households = "
                  << timer.elapsed().count() << " milliseconds.\n";
        
    }
}

std::unordered_map<long, long>
ScheduleGenerator::assignWorkBuildings(const OSMData& model,
                                       const Building& bld,
                                       BuildingMap& nonHomeBuildings,
                                       BuildingList& candidateWorkBlds,
                                       TrvlTimePeopleMap& timePeopleMap,
                                       const int timeMargin) {
    std::unordered_map<long, long> workBuildingAssignment;    
    // To speed up the assignment, a building will be assigned to
    // a person if the travel time to that building is between
    // travelTime - timeMargin and travelTime for that person
    int bldCount = 0;
    for (Building& wrkBld : candidateWorkBlds) {
        if (wrkBld.population < 1) {
            continue;  // No space left in work building
        }
        bldCount++;
        PathFinder pf(model);
        const Path path = pf.findBestPath(bld.id, wrkBld.id,
                                          true, 0.25, 0.1);
        if (path.size() == 0) {
            continue;
        }
        int timeInMinutes = (int)(std::round(path.back().distance * 60));
        for (int curMargin = 0; curMargin <= timeMargin; curMargin++) {
            const int curTime = timeInMinutes + curMargin;
            if (timePeopleMap.find(curTime) != timePeopleMap.end()) {
                // We have found a person to whom we can fuzzily
                // assign this building
                workBuildingAssignment[timePeopleMap.at(curTime).back()] =
                    wrkBld.id;
                wrkBld.population--;
                // Also update the actual building entry
                nonHomeBuildings[wrkBld.id].population--;
                std::cout << "Assign Building " << wrkBld.id
                          << " to person "
                          << timePeopleMap.at(curTime).back()
                          << " after checking " << bldCount << " out of "
                          << candidateWorkBlds.size() << " buildings. ";
                std::cout << "Person needs time: " << curTime
                          << " and the assigned building has time: "
                          << timeInMinutes << std::endl;
                timePeopleMap.at(curTime).pop_back();
                if (timePeopleMap.at(curTime).empty()) {
                    timePeopleMap.erase(curTime);
                }
            }
        }
        if (timePeopleMap.empty()) {
            break;
        }
    }
    
    ASSERT(timePeopleMap.empty());
    return workBuildingAssignment;
}


std::unordered_map<long, long>
ScheduleGenerator::assignWorkBuildings(const OSMData& model,
                                       const Building& bld,
                                       BuildingMap& nonHomeBuildings,
                                       BuildingList& candidateWorkBlds,
                                       const PUMSPerson& person,
                                       const int timeMargin) {
    // To speed up the assignment, a building will be assigned to
    // a person if the travel time to that building is between
    // travelTime - timeMargin and travelTime for that person
    int bldCount = 0;
    const int travelTime = person.getIntegerInfo(cmdLineArgs.jwmnpIdx);
    for (Building& wrkBld : candidateWorkBlds) {
        if (wrkBld.population < 1) {
            continue;  // No space left in work building
        }
        bldCount++;
        PathFinder pf(model);
        const Path path = pf.findBestPath(bld.id, wrkBld.id,
                                          true, 0.25, 0.1);
        if (path.size() == 0) {
            continue;
        }
        int timeInMinutes = (int)(std::round(path.back().distance * 60));
        if (std::abs(travelTime - timeInMinutes) > timeMargin) {
            // std::cout << "    Path time is " << timeInMinutes << ", expected: "
            //           << travelTime << std::endl;
            continue;  // This building is not acceptable
        }

        // Found a matching building
        wrkBld.population--;
        // Also update the actual building entry
        nonHomeBuildings[wrkBld.id].population--;
        std::cout << "Assign Building " << wrkBld.id << "(in ring: "
                  << wrkBld.attributes << ") to person " << person.getPerID()
                  << " in building " << bld.id << "(in ring: " << bld.attributes
                  << ") after checking " << bldCount << " out of "
                  << candidateWorkBlds.size() << " buildings. ";
        std::cout << "Person needs time: " << travelTime
                  << " and the assigned building has time: "
                  << timeInMinutes << std::endl;
        return {{person.getPerID(), wrkBld.id}};
    }
    std::cerr << "Unable to find suitable work building from "
              << candidateWorkBlds.size()
              << " candidate buildings for person ";
    person.write(std::cerr);
    return {};
}


Ring
ScheduleGenerator::getBldRing(int bldId, const Building& bld,
                          const std::string& infoKey) const {
    // Get info to use as population for the building
    const int info = bld.getInfo(infoKey);
    // Create the top-left and bottom-right points.
    const Point topLeft(bld.topLon, bld.topLat),
        botRight(bld.botLon, bld.botLat);
    const std::string entInfo = "bldId: " + std::to_string(bld.id) +
        "--" + infoKey + ": " + std::to_string(info);
    const std::vector<Ring::Info> infoList = {{1, "info",  entInfo }};
    // Create a ring for this building.
    return Ring(topLeft, botRight, info, bldId, bldId, infoList);
}

std::string
ScheduleGenerator::getColumn(const std::string& line, const int column,
                      const char delim) {
    int currCol = 0, startPos = 0;
    while (currCol < column) {
        startPos = line.find(delim, startPos + 1) + 1;
        currCol++;
    }
    int nextTab = line.find(delim, startPos + 1);
    return line.substr(startPos, nextTab - startPos);
}

int
ScheduleGenerator::drawShapeFiles(XFigHelper& fig, const StringList& shapeFiles,
                              const StringList& dbfFiles,
                              int& minXclip, int& minYclip,
                              const int figScale, int startShapeLevel) {
                              
    // The first shapefile becomes the primary. Other shape files are
    // intersected with this file to make the figure better.
    std::vector<ShapeFile> shapes;

    // The folllowing minXclip and minYclip are updated in loop below
    // to track the minimum values from all the shape files being
    // drawn.
    minXclip = minYclip = INT_MAX;

    // Below we accummulate all the shapes into a vector so that we
    // can determine the overall XClip and YClip values to know what
    // coordinate get's anchored to the top-left corner in XFig.  This
    // is needed because different shape files can end up having
    // different bounds.
    for (size_t i = 0; (i < shapeFiles.size()); i++) {
        // A temporary object to load shapes.
        ShapeFile shpFile;
        // Load data from the shapefile and dbf file.
        if (!shpFile.loadShapes(shapeFiles[i], dbfFiles[i])) {
            std::cerr << "Error loading shape/DBF file: "
                      << shapeFiles[i] << " or " << dbfFiles[i] << std::endl;
            return 3;
        }
        // Set default label information for each entry in the ring.
        if (!cmdLineArgs.labelColNames.empty()) {
            shpFile.setLabels(cmdLineArgs.labelColNames);
        }
        // If this is not the first shape file then intersect with
        // shapes in the first one to retain only relevant shapes to
        // keep the figure compact.
        if (!shapes.empty()) {
            std::cout << "Intersecting shape file " << shapeFiles[i]
                      << std::endl;
            shpFile = intersections(shapes.front(), shpFile);
        }
        // Add the new shape to the shapes to be drawn.
        shapes.push_back(shpFile);

        // Update the clipping bounds based on the new shape
        int xClip = -1, yClip = -1;
        shpFile.getClipBounds(figScale, xClip, yClip);
        minXclip = std::min(minXclip, xClip);
        minYclip = std::min(minYclip, yClip);
    }

    // Now write all the shapes to the specified xfig with a fixed
    // clipping bounds.
    const std::vector<int> Colors = { BLACK, RED, BLUE, GREEN, CYAN };
    for (size_t i = 0; (i < shapes.size()); i++) {
        shapes[i].genXFig(fig, minXclip, minYclip, figScale,
                          false, {}, false, shapeFiles[i],
                          startShapeLevel + (i * 10), Colors.at(i));
    }
    // Everything went well
    return 0;
}

int
ScheduleGenerator::processArgs(int argc, char *argv[]) {
    // Make the arg_record to process command-line arguments.
    ArgParser::ArgRecord arg_list[] = {
        {"--shapes", "List of shape files to be drawn",
         &cmdLineArgs.shapeFilePaths, ArgParser::STRING_LIST },
        {"--dbfs", "List of associated DBF file to be used for metadata",
         &cmdLineArgs.dbfFilePaths, ArgParser::STRING_LIST },
        {"--xfig", "The output XFig file",
         &cmdLineArgs.xfigFilePath, ArgParser::STRING},
        {"--scale", "The size of the output map",
         &cmdLineArgs.figScale, ArgParser::INTEGER},
        {"--label-cols", "Shapes metadata columns to use for labels",
         &cmdLineArgs.labelColNames, ArgParser::STRING_LIST},
        {"--key-info", "Key building info to plot [households, people]",
         &cmdLineArgs.key_info, ArgParser::STRING},
        {"--model", "The model text file for building data",
         &cmdLineArgs.modelFilePath, ArgParser::STRING},
        {"--avg-speed", "Avg. miles/hr to use for approx. office",
         &cmdLineArgs.avgSpeed, ArgParser::INTEGER},
        {"--jwmnp-idx", "Index of JWMNP (work travel time) column in model",
         &cmdLineArgs.jwmnpIdx, ArgParser::INTEGER},
        {"--jwtrns-idx", "Index of JWTRNS (means of travel) column in model",
         &cmdLineArgs.jwtrnsIdx, ArgParser::INTEGER},        
        {"--off-sqFt-pp", "Office sq.ft per person",
         &cmdLineArgs.offSqFtPer, ArgParser::INTEGER},
        {"--num-bld-pairs", "Number of building-paris to estimate travel time",
         &cmdLineArgs.numBldPairs, ArgParser::INTEGER},
        {"--out-trvl-est", "Output file to store travel estimate matrix",
         &cmdLineArgs.outTrvlEstFile, ArgParser::STRING},
        {"--use-trvl-est", "Input file to read travel estimate matrix",
         &cmdLineArgs.useTrvlEstFile, ArgParser::STRING},
        {"", "", NULL, ArgParser::INVALID}
    };
    // Process the command-line arguments.
    ArgParser ap(arg_list);
    ap.parseArguments(argc, argv, true);
    // Ensure at least the model file and XFig files are specified.
    if (cmdLineArgs.modelFilePath.empty()) {
        std::cerr << "Must specify an input model file file.\n";
        return 1;
    }

    // Ensure that the number of shapes and dbf files match up
    if (cmdLineArgs.shapeFilePaths.size() != cmdLineArgs.dbfFilePaths.size()) {
        std::cerr << "The number of shapes & dbf files do not match\n";
        return 2;
    }

    // Things seem fine so far
    return 0;
}

ShapeFile
ScheduleGenerator::intersections(const ShapeFile& primary,
                             const ShapeFile& secondary) const {
    // The resulting shapefile with shapes from secondary that
    // intersect with the primary.
    ShapeFile intersection;
#pragma omp parallel for schedule(guided)
    for (int secIdx = 0; (secIdx < secondary.getRingCount()); secIdx++) {
        // Get the ring that we are working with
        const Ring& secRing = secondary.getRing(secIdx);
        // Check intersection between secondary ring with all primary rings.
        for (int priIdx = 0; (priIdx < primary.getRingCount()); priIdx++) {
            const Ring& priRing = primary.getRing(priIdx);
            if (priRing.contains(secRing) || secRing.intersects(priRing)) {
                // Found an intersection. Copy the secondary ring to
                // our resulting shape file.
#pragma omp critical (intersect)
                intersection.addRing(secRing);
            }
        }
    }  // omp parallel

    // Return intersecting shapes back.
    return intersection;
}

int main(int argc, char *argv[]) {
    ScheduleGenerator drawer;
    return drawer.run(argc, argv);
}

#endif
