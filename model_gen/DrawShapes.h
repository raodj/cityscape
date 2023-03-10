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
#include "ShapeFile.h"
#include "ArgParser.h"

/** A shortcut to a pair that holds the key and value */
using KeyValue = std::pair<std::string, std::string>;

class DrawShapes {
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

    /** Write pertinent community and population rings in XFIG format
        to a given file -- assuming an output fig file was specified
        via the \c --xfig command-line argument.
    */
    void generateFig();

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

    /**
       This methods updates the shapes with additional data from the
       specified annotation file so the rings are colored
       appropriately.

       \param[in] annotPath Path to the annotation TSV to be used.

       \param[in] areaColIdx The column in the annotation-TSV that has
       the area ID value for identifying the rings the shape file.

       \param[in] colorColIdx The column in the anntation-TSV that has
       the color ID value to be used.  This column is assumed to be an
       integer.
    */
    void addAnnotations(const std::string& annotPath,
                        const int areaColIdx, const int colorColIdx);
    
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

        /** Path to the output XFIG figure file to which the model is
            to be written for visual crossvalidation */
        std::string xfigFilePath;
        
        /** The scale for the output XFIG figure so that various
            shapes and information are readable
        */
        int figScale = 1638400;

        /** Path to a TSV file that contains additional annotations to
            be used for each ring loaded from the shape file.
         */
        std::string annotFile;

        /** The zero-based column index in the annotation file that
           contains the area code that is used to associate each line
           of the annotation with corresponding ring in the shapes
           file.
        */
        int annotAreaNumCol = 0;
        
        /** The zero-based column index in the annotation file to be
            used to determine color codes for each ring.  It is
            assumed that this column has an integer.
        */
        int annotColorCol = -1;
        
        /** A list of column names in the community shape file that
            are to be included in the XFIG file to ease visual cross
            referencing.
        */
        ArgParser::StringList labelColNames = {"area_numbe", "community"};

    } cmdLineArgs;

    /** This is just a copy of the command-line arguments supplied to
        the run method.  This vector is used to write the command-line
        arguments used to generate a model to a given model file.
    */
    std::string actualCmdLineArgs;
};

#endif
