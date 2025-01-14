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
#include "Utilities.h"
#include "ScheduleGenerator.h"
#include "ShapeFile.h"
#include "OSMData.h"
#include "PathFinder.h"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <utility>

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

std::pair<BuildingList, BuildingList>
ScheduleGenerator::getHomeAndNonHomeBuildings(const BuildingMap& buildingMap) const {
    BuildingList homeBuildings, nonHomeBuildings;
    for (auto item = buildingMap.begin(); item != buildingMap.end(); item++) {
        if (item->second.isHome) {
            homeBuildings.push_back(item->second);
        } else {
            nonHomeBuildings.push_back(item->second);
        }
    }
    
    ASSERT(homeBuildings.size() + nonHomeBuildings.size() ==
           buildingMap.size());
    
    std::cout << "# of non-home buildings: " << nonHomeBuildings.size() << '\n';
    std::cout << "# of home buildings: " << homeBuildings.size() << std::endl;

    return {homeBuildings, nonHomeBuildings};
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
                                            const BuildingList& nonHomeBlds,
                                            const int minTravelTime,
                                            const int maxTravelTime,
                                            const int timeMargin) const {
    BuildingList candidateBlds;

    for (const  Building& bld : nonHomeBlds) {
        // Get the distance in miles from the source building to an
        // non-office building.
        const double dist = getDistance(srcBld.wayLat, srcBld.wayLon,
                                        bld.wayLat,    bld.wayLon);
        // Convert distance to time using an "average" estimated time.
        // This approach is used because computing actual path is
        // slower.
        const int timeInMinutes = std::round(dist / cmdLineArgs.avgSpeed);

        // Use only buildings that are within our specified time ranges
        if ((timeInMinutes >= (minTravelTime - timeMargin)) &&
            (timeInMinutes <= (maxTravelTime + timeMargin))) {
            candidateBlds.push_back(bld);
        }
    }

    // Sort the candidate non-home buildings based on their distances
    // to current building (approximating travel time)
    std::sort(candidateBlds.begin(), candidateBlds.end(),
              [&srcBld](const Building& b1, const Building& b2) {
                  return getDistance(srcBld.wayLat, srcBld.wayLon,
                                     b1.wayLat, b1.wayLon) > 
                      getDistance(srcBld.wayLat, srcBld.wayLon,
                                  b2.wayLat, b2.wayLon);
              });
    
    std::cout << "# of candidate work locations: " << candidateBlds.size()
              << std::endl;
    return candidateBlds;
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


void
ScheduleGenerator::generateSchedule(const OSMData& model, XFigHelper& fig,
                                    const std::string& infoKey) {
    std::cout << "Generating Schedule..." << std::endl;

    // Classify buildings by their types in order to reduce the number
    // of buildings we iterate on for different operations.
    auto [homeBuildings, nonHomeBuildings] =
        getHomeAndNonHomeBuildings(model.buildingMap);
    
    for (auto& bld : homeBuildings) {
        std::cout << "Current Building ID: " << bld.id << std::endl;
        const int travelTimeIdx = 2, meansOfTransportationIdx = 3;
        const auto [minTravelTime, maxTravelTime] = 
            findLongestShortestToWorkTime(bld, cmdLineArgs.jwmnpIdx,
                                          cmdLineArgs.jwtrnsIdx);

        if (minTravelTime <= 0) {
            // If the travel time is negative, it means there is
            // nothing to do...
            continue;
        }

        
        // Get all people info of the current building
        std::unordered_map<int, std::vector<long>> timePeopleMap;
        for (size_t i = 0; i < bld.households.size(); i++) {
            const auto& curHousehold = bld.households.at(i);
            const auto& peopleInCurHousehold = curHousehold.getPeopleInfo();

            for (size_t j = 0; j < peopleInCurHousehold.size(); j++) {
                const PUMSPerson& curPerson = peopleInCurHousehold.at(j);

                int travelTimeToWork, transportMeans;

                try {
                    travelTimeToWork = curPerson.getIntegerInfo(travelTimeIdx);
                    transportMeans = curPerson.getIntegerInfo(meansOfTransportationIdx);
                } catch (const std::out_of_range& e) {
                    std::cerr << "Skipping person with ID: " << curPerson.getPerID() << " due to insufficient amount of data" << std::endl;
                    continue;
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Skipping person with ID: " << curPerson.getPerID() << " because either travel time to work or transportation means is not an integer" << std::endl;
                    continue;
                }

                if (transportMeans != 1 || travelTimeToWork <= 0) {
                    continue;
                }

                if (timePeopleMap.find(travelTimeToWork) == timePeopleMap.end()) {
                    timePeopleMap[travelTimeToWork] = std::vector<long>{};
                }

                timePeopleMap.at(travelTimeToWork).push_back(curPerson.getPerID());
            }
        }

        // // Sort people in current building by travel-to-work time
        // std::sort(people.begin(), people.end(), 
        // [travelTimeIdx](const PUMSPerson& p1, const PUMSPerson& p2) {
        //     // Calls to getIntegerInfo here are guaranteed to not throw any exceptions
        //     // because otherwise the control would not reach here
        //     return (p1.getIntegerInfo(travelTimeIdx) < p2.getIntegerInfo(travelTimeIdx)); 
        // });
        // std::cout << "Finish sorting people" << std::endl;

        for (auto& it : timePeopleMap) {
            std::cout << it.first << ": " << it.second.size() << " people [ ";
            for (long l : it.second) {
                std::cout << l << " ";
            }
            std::cout << "]" << std::endl;
        }

        const int timeMargin = 1;  // Wiggle room of 1 minute
        const BuildingList possibleWorkBuildingsForCurBuilding =
            getCandidateWorkBuildings(bld, nonHomeBuildings, minTravelTime,
                                      maxTravelTime, timeMargin);
        std::unordered_map<long, long> workBuildingAssignment;
        // To speed up the assignment, a building will be assigned to a person
        // if the travel time to that building is between travelTime - timeMargin
        // and travelTime for that person

        for (size_t j = 0; j < possibleWorkBuildingsForCurBuilding.size() && !timePeopleMap.empty(); j++) {
            PathFinder pf(model);
            const Path path = pf.findBestPath(bld.id, possibleWorkBuildingsForCurBuilding.at(j).id, true, 0.25, 0.1);

            if (path.size() == 0) {
                continue;
            }

            int timeInMinutes = (int)(std::round(path.back().distance * 60));

            for (int curMargin = 0; curMargin <= timeMargin; curMargin++) {
                const int curTime = timeInMinutes + curMargin;
                if (timePeopleMap.find(curTime) != timePeopleMap.end()) {
                    // We have found a person to whom we can fuzzily assign this building
                    workBuildingAssignment[timePeopleMap.at(curTime).back()] = possibleWorkBuildingsForCurBuilding.at(j).id;

                    // Erase the assigned building from the list
                    for (auto it = nonHomeBuildings.begin(); it != nonHomeBuildings.end(); it++) {
                        if (it->id == possibleWorkBuildingsForCurBuilding.at(j).id) {
                            nonHomeBuildings.erase(it);
                            break;
                        }
                    }
                    std::cout << "Assign Building " << possibleWorkBuildingsForCurBuilding.at(j).id << " to person " << timePeopleMap.at(curTime).back() << std::endl;
                    std::cout << "Person needs time: " << curTime << " and the assigned building has time: " << timeInMinutes << std::endl;
                    timePeopleMap.at(curTime).pop_back();
                    if (timePeopleMap.at(curTime).empty()) {
                        timePeopleMap.erase(curTime);
                    }
                    break;
                }
            }
        }

        ASSERT(timePeopleMap.empty());

        // for (size_t k = 0; k < people.size(); k++) {
        //     const PUMSPerson& curPerson = people.at(k);
        //     int prevTime = INT_MAX;

        //     for (size_t bi = 0; bi < possibleWorkBuildingsForCurBuilding.size(); bi++) {
        //         PathFinder pf(model);
        //         const Path path = pf.findBestPath(bld.id, possibleWorkBuildingsForCurBuilding.at(bi).id, true, 0.25, 0.1);
                
        //         if (path.size() == 0) {
        //             continue;
        //         }

        //         const int timeInMinutes = (int)(std::round(path.back().distance * 60));
        //         std::cout << "Time:" << 

        //         // if (timeInMinutes == curPerson.getIntegerInfo(travelTimeIdx)) {
        //         //     workBuildingAssignment[curPerson.getPerID()] = possibleWorkBuildingsForCurBuilding.at(bi).id;
        //         //     std::cout << "Assign Person " << curPerson.getPerID() << " to " << possibleWorkBuildingsForCurBuilding.at(bi).id << std::endl;
        //         //     break;
        //         // } else if (timeInMinutes > curPerson.getIntegerInfo(travelTimeIdx)) {
        //         //     std::cout << "\rTravel time too high: expected = " << curPerson.getIntegerInfo(travelTimeIdx) << ", got = " << timeInMinutes;
        //         //     // std::cout << "Fail to assign Person with travel time: " << curPerson.getIntegerInfo(travelTimeIdx) << ". The closest one is " << timeInMinutes << " minutes" << std::endl;
        //         // } else {
        //         //     std::cout << std::endl;
        //         //     std::cout << "Fail to assign Person from home ID: " << bld.id << " with travel time: " << curPerson.getIntegerInfo(travelTimeIdx) << ". The closest one is " << timeInMinutes << " minutes with ID: " << possibleWorkBuildingsForCurBuilding.at(bi).id << std::endl;
        //         //     break;
        //         // }
        //     }
        // }

        // long assignedBuildingID, shortestDistance = LONG_MAX;

        // #pragma omp parallel for
        // for (size_t k = 0; k < possibleWorkBuildingsForCurBuilding.size(); k++) {
        //     const Building& curPossibleBuilding = possibleWorkBuildingsForCurBuilding.at(k);
        //     PathFinder pf(model);
        //     const Path path = pf.findBestPath(bld.id, curPossibleBuilding.id, true, 0.25, 0.1);

        //     if (path.size() == 0) {
        //         continue;
        //     }

        //     const int timeInMinutes = (int)(std::round(path.back().distance * 60));
        //     // const int distanceInMiles = pf.findBestPath(bld.id, curPossibleBuildingID, false, 0.25, 0.1).back().distance;
        //     const int distance = getDistance(bld.wayLat, bld.wayLon, curPossibleBuilding.wayLat, curPossibleBuilding.wayLon);

        //     #pragma omp critical
        //     {
        //         if (timeInMinutes == travelTimeToWork && shortestDistance > distance ) {
        //             assignedBuildingID = curPossibleBuilding.id;
        //             shortestDistance = distance;
        //         }
        //     }
        // }

        // workBuildingAssignment[curPerson.getPerID()] = assignedBuildingID;

        // std::cout << "Person ID: " << curPerson.getPerID() << " Assigned Building: " << assignedBuildingID << std::endl;
    }
}

/* generateSchedule(person, listOfBuildings)
get that person's travel time to work
add the four colums when generating the model to know the travel type (to work)
print info for each person to make sure correction
Find non-home building (isHome == false) 
whose travel time == the person's travel time to work

Assign the person to work at the building

*/

// int
// ScheduleGenerator::generateSchedule(const PUMSPerson& person, const std::unordered_map<long, Building> buildingMap) {
    

//     // std::cout << "Person ID: " << person.getPerID() << "\n" << "Info string: " << person.getInfo() << std::endl;


//     // const auto travelTimeToWorkCol = std::find(PUMSPerson::colTitles.begin(), PUMSPerson::colTitles.end(), "JWMNP");
//     int travelTimeToWorkIdx = 2;
//     // if (travelTimeToWorkCol != PUMSPerson::colTitles.end()) {
//     //     travelTimeToWorkIdx = travelTimeToWorkCol - PUMSPerson::colTitles.begin();
//     // } else {
//     //     std::runtime_error("Generating schedule for a person requires the column \"JWMNP\"");
//     //     return -1;
//     // }

//     // const auto meansOfTransportationCol = std::find(PUMSPerson::colTitles.begin(), PUMSPerson::colTitles.end(), "JWTRNS");
//     int meansOfTransportationIdx = 3;
//     // if (meansOfTransportationCol != PUMSPerson::colTitles.end()) {
//     //     meansOfTransportationIdx = meansOfTransportationCol - PUMSPerson::colTitles.begin();
//     // } else {
//     //     std::runtime_error("Generating schedule for a person requires the column \"JWTRNS\"");
//     //     return -1;
//     // }

//     int travelTimeToWork;
//     int meansOfTransportationID;

//     try {
//         travelTimeToWork = std::stoi(personInfo.at(travelTimeToWorkIdx));
//     } catch () {
//         std::cerr << "Cannot generate schedule for person " << person.getPerID() << " due to non-numerical value for travel time to work" << std::endl;
//         return -1;
//     }

//     try {
//         meansOfTransportationID = std::stoi(personInfo.at(meansOfTransportationIdx));
//     } catch (const std::invalid_argument& e) {
//         std::runtime_error("Means of transportation (JWTRNS column) for person " + std::to_string(person.getPerID()) + " is not an integer");
//         return -1;
//     }

//     // We are good to do the actual generation now
//     if (meansOfTransportationID != 1 || travelTimeToWork <= 0) {
//         // We only care about bus, car, or van as transportation
//         // means, i.e. JWTRNS == 1
//         return -1;
//     }

//     std::cout << "Person ID = " << person.getPerID() << " TimeToWork = " << travelTimeToWork << std::endl;

//     return -1;
// }

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
        {"", "", NULL, ArgParser::INVALID}
    };
    // Process the command-line arguments.
    ArgParser ap(arg_list);
    ap.parseArguments(argc, argv, true);
    // Ensure at least the model file and XFig files are specified.
    if (cmdLineArgs.modelFilePath.empty() || cmdLineArgs.xfigFilePath.empty()) {
        std::cerr << "Specify a model file and a xfig file.\n";
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
