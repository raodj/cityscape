#ifndef DRAW_SHAPES_H
#define DRAW_SHAPES_H

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
#include "XFigHelper.h"
#include "ArgParser.h"
#include "ShapeFile.h"
#include "OSMData.h"

/** A shortcut to a pair that holds the key and value */
using KeyValue = std::pair<std::string, std::string>;

/** An alias to streamline code below */
using StringList = ArgParser::StringList;

/**
 * This top-level class was added to draw buildings based on:
 * <ol>
 *   <li> The number of household or people in the building.</li>
 *   <li> The average income of the building</li>
 * </ol>
 */
class DrawBuildings {
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

    int getInfo(const Building& bld, const std::string& infoKey) const;
    
    void drawBuildings(const OSMData& model, XFigHelper& fig,
                       const std::string& infoKey, int xClip, int yClip);

    Ring getBldRing(int bldId, const Building& bld,
                    const std::string& infoKey) const;
    
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

        
        /** Flag to indicate if building rings from the model are to
            be drawn.  This flag is enabled by the --draw-buildings
            command-line option.
        */
        bool drawBuildings = false;

        /** Flag to indicate if community shapes (from shapefiles) are
            to be drawn.  This flag is enabled by the --draw-communities
            command-line option.
        */
        bool drawCommunities = false;

        /** Flag to indicate if population rings from the model are to
            be drawn.  This flag is enabled by the --draw-populations
            command-line option.
        */
        bool drawPopulations = false;

        /** The path to the model text file from where building data
            is to be obtained.
         */
        std::string modelFilePath;
        
    } cmdLineArgs;

    /** This is just a copy of the command-line arguments supplied to
        the run method.  This vector is used to write the command-line
        arguments used to generate a model to a given model file.
    */
    std::string actualCmdLineArgs;
};

#endif
