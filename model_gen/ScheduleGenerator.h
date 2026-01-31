#ifndef SCHEDULE_GENERATOR_H
#define SCHEDULE_GENERATOR_H

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
#include <tuple>
#include "XFigHelper.h"
#include "ArgParser.h"
#include "ShapeFile.h"
#include "OSMData.h"
#include "LinearWorkBuildingAssigner.h"

/** A shortcut to a pair that holds the key and value */
using KeyValue = std::pair<std::string, std::string>;

/** An alias to streamline code below */
using StringList = ArgParser::StringList;

/** An alias to refer to a vector of buildings **/
using BuildingList = std::vector<Building>;

/** An alias for a map of buildings with the building ID as the key */
using BuildingMap = std::unordered_map<long, Building>;

/** An synonym for a map with travel time (in minutes) as the key and
    the list of peopleIDs as the values */
using TrvlTimePeopleMap = std::unordered_map<int, std::vector<long>>;
    
/**
 * This top-level class was added to draw buildings based on:
 * <ol>
 *   <li> The number of household or people in the building.</li>
 *   <li> The average income of the building</li>
 * </ol>
 */
class ScheduleGenerator {
public:
    /** The top-level method that coordinates the various tasks.
        
        This method essentially calls various internal helper methods
        in the following order:

        <ol>

        <li>First it calls loadShapeFile method which loads the
        specified shape files.  Note that mutiple shape files can be
        specified.  Typically, the community shape file and PUMA shape
        files are specified.</li>

        <li>Next it loads the specified model using an OSMData object.
        This model contains information about the buildings and the
        households in the buildings.</li>

        <li>Then the program generates the XFig with shapes and
        buildings<li>
        
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

    /** Loads and draws the shape files and DBF file from the
        specified input files.

        This method is typically the first method that is invoked to
        load the shape files (community, PUMA, etc.) and associated
        metadata from the the given command-line arguments.  It writes
        pertinent community and population rings in XFIG format to a
        given file specified via the \c --xfig command-line argument.

        \param[out] fig The XFig object to be used to draw shapes.
        
        \param[in] shapeFiles The list of shape files to be drawn.

        \param[in] dbfFiles The DBF files corresponding to each shape
        to be drawn.
        
        \return This method returns zero if the command-line arguments
        were successfully processed.  On errors it returns a non-zero
        error code.        
    */
    int drawShapeFiles(XFigHelper& fig, const StringList& shapeFiles,
                       const StringList& dbfFiles,
                       int& xClip, int& yClip,
                       const int figScale = 4096, int startShapeLevel = 100);
                       

    /**
       Helper method to get a specific column from a given string
       based on a given delimiter.

       \param[in] line The line from where a given column is to be
       extracted.

       \param[in] column The zero-based index of the column to be
       returned.

       \param[in] delim An optional delimiter character. The default
       is a tab.
    */
    std::string getColumn(const std::string& line, const int column,
                          const char delim = '\t');

protected:
    /** Helper method to return intersecting shapes between a primary
        and secondary set of shapes.

        \param[in] primary The primary shape file to be used for
        intersections.

        \param[in] secondary The secondary shape file to be used for
        intersections.
    */
    ShapeFile intersections(const ShapeFile& primary,
                            const ShapeFile& secondary) const;

    /**
       Just a convenience method to return the information associated
       with a building.

       \param[in] bld The building for which information is to be returned.

       \param[in] infoKey The key that is used to identify the
       information to be returned.  The key can be one of: "people",
       "households", or "avg_income".

       \return The information associated with the building based on
       the key.
     */
    int getInfo(const Building& bld, const std::string& infoKey) const {
        return bld.getInfo(infoKey);
    }
    
    void generateSchedule(const OSMData& model, XFigHelper& fig,
                          const std::string& infoKey, int argc, char *argv[]);
                       
    int generateSchedule(const PUMSPerson& person,
                         const std::unordered_map<long, Building> buildingMap);

    Ring getBldRing(int bldId, const Building& bld, const std::string& infoKey) const;

    /**
     * This is just an internal hepler method that is used to draw the
     * necessary model information to an XFig file for visualization. This
     * method is invoked from run() only if the the xfigFile path is
     * secpfied as an command-line argument.
     */
    int drawXfig(XFigHelper& xfig);

    /**
       An internal convenience method to make a list of home and
       non-home buildings so that we iterate on a smaller subset of
       lists (and avoid checks) to make things a tad-bit faster.

       \param[in] buildingMap The map of the buildings from the model.

       \return A pair of building lists with the home and non-home
       buildings (in that order).
     */
    std::tuple<BuildingMap, BuildingMap, BuildingList>
    getHomeAndNonHomeBuildings(const BuildingMap& buildingMap) const;

    /**
       This is a helper method that is used to compute the shortest
       and longest work-travel time for all the people in a given
       building, whose means of transport is 1. See list of
       transportation means (JWTRNS) from
       https://www2.census.gov/programs-surveys/acs/tech_docs/pums/data_dict/PUMS_Data_Dictionary_2021.pdf
       below:

       bb .N/A (not a worker-not in the labor force, including persons
       .under 16 years; unemployed; employed, with a job but not at
       .work; Armed Forces, with a job but not at work)
       01 .Car, truck, or van
       02 .Bus
       03 .Subway or elevated rail
       04 .Long-distance train or commuter rail
       05 .Light rail, streetcar, or trolley
       06 .Ferryboat
       07 .Taxicab
       08 .Motorcycle
       09 .Bicycle
       10 .Walked
       11 .Worked from home
       12 .Other method
       
       \param[in] bld The building for which which people's shortest
       and longest work-travel time is to be computed.

       \param[in] travelTimeIdx The index of the JWMNP attribute with
       travel time.  The default value for this is 2.

       \param[in] meansOfTransportationIdx The index of the JWTRNS
       attribute of a person that indicates their means of
       transportation.  The default index is 3.
     */
    std::pair<int, int>
    findLongestShortestToWorkTime(const Building& bld,
                                  int travelTimeIdx = 2,
                                  int meansOfTransportationIdx = 3) const;

    /**
       Internal helper method to return a sorted list of buildings
       that are potential candidates to the given building.

       \param[in] bld The source building from which non-home
       buildings are to be obtained.

       \param[in] nonHomeBlds The list of non-home buildings from
       where the subset of buildings are to be returned.

       \param[in] minTravelTime The minimum anticipated travel time
       (in minutes).

       \param[in] maxTravelTime The maximum anticipated travel time
       (in minutes)

       \param[in] timeMargin Additional fudge factor that is used to
       find buildings around the specified limit (in minutes).
     */
    BuildingList getCandidateWorkBuildings(const Building& bld,
                                           const BuildingMap& nonHomeBlds,
                                           const BuildingList& nonHomeBldsList,
                                           const int minTravelTime,
                                           const int maxTravelTime,
                                           const int timeMargin = 1) const;

    /**
       Obtain a map of people who all have the same travel time
       (rounded to the nearest minute) to work.

       \param[in] bld The building for which the travel time of all
       the people in the building are to be returned.
    */
    TrvlTimePeopleMap
    getTimePeopleMap(const Building& bld) const;

    std::unordered_map<long, long>
    assignWorkBuildings(const OSMData& model,
                        const Building& bld,
                        BuildingMap& nonHomeBuildings,
                        BuildingList& candidateWorkBlds,
                        TrvlTimePeopleMap& timePeopleMap,
                        const int timeMargin);

    std::unordered_map<long, long>
    assignWorkBuildings(const OSMData& model,
                        const Building& bld,
                        BuildingMap& nonHomeBuildings,
                        BuildingList& candidateWorkBlds,
                        const PUMSPerson& person,
                        const int timeMargin);
private:
    /** This is a simple inner class that is used to conveniently
        encapsulate various command-line arguments that is used by the
        model generator.  This encapsulation streamlines managing
        several different command-line arguments in a streamlined
        manner
    */
    class CmdLineArgs {
    public:
        /** The path to the shape files from where shapes to be drawn
            are to be loaded. */
        ArgParser::StringList shapeFilePaths;

        /** The path to the DBF file corresponding to each shapeFile
            that contains metadata for each shape file. */
        ArgParser::StringList dbfFilePaths;

        /** A list of column names in the shapes files that are to be
            included in the XFIG file to ease visual cross
            referencing.
        */
        ArgParser::StringList labelColNames = {"area_numbe", "community"};
        
        /**
           The starting level in XFig for shapes. Each shape skips 10
           levels to ease isolating them in XFig for visualization.
        */
        int startShapeLevel = 100;
        
        /** Path to the output XFIG figure file to which the model is
            to be written for visualization and validation */
        std::string xfigFilePath;
        
        /** The scale for the output XFIG figure so that various
            shapes and information are readable
        */
        int figScale = 1638400;
        
        /** The key attribute/information based on which the buildings
            are to be color coded.  The legal values are: "people",
            "households", "avg_income".
        */
        std::string key_info = "avg_income";

        /** The path to the model text file from where building data
            is to be obtained.
         */
        std::string modelFilePath;

        /**
         * This is the speed value that is used to compute approximate
         * travel time between a home and an office building.  This is
         * used to filter out possible locations as finding exact path
         * between buildings is a lot more time consuming
         * (i.e. milliseconds vs microseconds)
         */
        int avgSpeed = 35;   // miles per hour

        /**
           The zero-based index of the "travel time to work" (jwmnp)
           column data in each person's information as embedded in a
           model.
         */
        int jwmnpIdx = 2;

        /**
           The zero-based index of the "means of transportation to
           work" (jwtrns)  column data in each person's information as embedded
           in a model.
         */
        int jwtrnsIdx = 3;

        /**
           The average square footage per person in an office building
           to estimate the number of people who work in an office
           building.
         */
        int offSqFtPer = 350;

        /**
           The number of building pairs to be used to estimate travel
           times between each pair of population rings.
        */
        int numBldPairs = 5;

        /**
           The number of building pairs to be used to estimate generate
           the linear model used for predicting haversine distance in 
           work-building assignment.
        */
        int lmNumSamples = 5000;

        /**
           The path to the output txt file to where generated travel
           estimates are to be written.  The generated file can be
           used in the future via the --use-trvl-est command-line
           argument.
        */
        std::string outTrvlEstFile;

        /**
           The path to the input text file from where travel estimates
           are to be read and used.  This file must be generated for
           exactly the same model using the --out-trvl-est
           command-line argument.
        */
        std::string useTrvlEstFile;
        
    } cmdLineArgs;

    /** This is just a copy of the command-line arguments supplied to
        the run method.  This vector is used to write the command-line
        arguments used to generate a model to a given model file.
    */
    std::string actualCmdLineArgs;
};

#endif
