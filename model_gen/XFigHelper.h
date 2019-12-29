
#ifndef XFIG_HELPER_H
#define XFIG_HELPER_H

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

#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <vector>

// Color codes for xfig
#define BLACK   0
#define BLUE    1
#define GREEN   2
#define CYAN    3
#define RED     4
#define MAGENTA 5
#define YELLOW  6
#define WHITE   7

// Color code offset from where user colors start
#define USER_COLOR_START 32
// The XFIG scale 1 in == 1200 xfig units
// #define XFIG_SCALE 1200

// The XFIG scale 15 units == 1 pixel
#define XFIG_SCALE 15

// Text Justification
#define LEFT_JUSTIFIED   0
#define CENTER_JUSTIFIED 1
#define RIGHT_JUSTIFIED  2

// Fill style constants
#define SOLID_FILL 20
#define NO_FILL    -1

class XFigHelper {
public:
    /** The constructor.
            
        The constructor merely initializes all the instance variables
        to their default initial value. Currently, this constructor is
        simple and is present merely to adhere to coding conventions.
    */
    XFigHelper();
    
    /** Method to set output stream and generate initial XFig header.
        
        \param[out] fileName The name of the file to which the XFig
        data is to be written.  This method attempts to open the file
        and write a standard XFig header to the file if needed.  If
        this process experiences errors then this method returns \c
        false.

	\param[in] genCustomColors If this flag is set to \c true then
	this method adds an user-defined color table to the generated
	XFig file. The user-defined color table guarantees a set of
	visibly different colors for use in applications.

	\param[in] appendFlag If this parameter is set to \c true,
	then this method assumes that the supplied file name already
	has XFig data in it and rest of the information needs to be
	just appended to the file. Consequently, this method does not
	generate any headers in this case.
	
        \return This method returns \c true if the file was opened and
        the header was successfully written.  On errors this method
        returns \c false.
    */
    bool setOutput(const std::string &fileName,
                   const bool genCustomColors = false,
		   const bool appendFlag      = false);

    /** Determine if the output file is in valid condition.

	This method can be used to determine if the output file is in
	valid condition.
	
	\return This method returns \c true if the output file is in
	valid condition.
    */
    bool good() const { return os.good(); }
    
    /** The destructor.
        
        Currently the destructor does not have any specific tasks to
        perform.  It is present merely to adhere to coding convetions.
    */
    ~XFigHelper();

    /** Draw a line in the XFig code.

        This method can be used to generate the code necessary to draw
        a line.

        \param[in] x1 The x-coordinate of the first point.

        \param[in] y1 The y-coordinate of the first point.

        \param[in] x2 The x-coordinate of the second point.

        \param[in] y2 The y-coordinate of the second point.

        \param[in] colorCode An optional color code to specify color
        of the line.
    */
    void drawLine(const int x1, const int y1,
                  const int x2, const int y2,
                  const int colorCode = 0, const int level = 50);

    /** Dump code for displaying a text

        This method can be used to generate code for printing a text
        (or string) at a specific location.

        \return Return height of text in xfig units!
    */
    int drawText(const std::string& text,
                 const int x, const int y,
		 const int justify = LEFT_JUSTIFIED,
		 const int fontCode = 14,
                 const int fontSize = 12,
                 const int colorCode = 0,
                 const int level = 50);

    /** Dump code for displaying a rectangle.

        This method can be used to generate a rectangle filled with a
        given color on the generated XFig file.

        \param[in] x The top-left x-coordinate for the rectangle (in
        XFig units).

        \param[in] y The top-left y-coordinate for the rectangle (in
        XFig units).        

        \param[in] width The width of the rectangle in XFig units.

        \param[in] height The height of the rectangle in XFig units.
    */
    void drawRect(int x, int y, int width, int height, int colorCode = 0,
		  int level = 50, int thickness = 1,
                  const int fillStyle = SOLID_FILL);

    void startPolygon(const int vertexCount, const int lineColor = BLACK,
                      const int fillColor = RED, const int level = 50,
                      const int fillStyle = 20);

    void startPolyLine(const int vertexCount, const int lineColor = BLACK,
		       const int level = 50);

    void addVertex(const int x, const int y);
    void endPolygon();
    void endPolyLine();

    void addImage(const std::string& fileName, const int x, const int y,
                  const int width, const int height, const int layer = 60);

    void drawOval(const int x, const int y,
                  const int xRad, const int yRad,
		  const int lineColor = BLACK,
		  const int fillColor = RED, const int level = 50,
		  const int fillStyle = 20);
    
    void addComment(const std::string& comment);

    void drawScaleBar(const int x, const int y,
                      const int width, const int height, 
                      std::vector<double> tics, const int layer = 60);

protected:
    /** Method to dump initial XFig header to the file.
        
        This is a helper method that must be used to dump a standard
        XFig header to the supplied output file.

	\param[in] genCustomColors If this flag is set to \c true then
	this method adds an user-defined color table to generated
	XFig. The user-defined color table guarantees a set of visibly
	different colors for use in applications.
    */
    void dumpHeader(const bool genCustomColors);

    /** Helper method generate color table.

        This method is invoked from the dumpHeader() method to
        actually generate the table of user-defined colors at the end
        of the header.
    */
    void generateColorTable();
    
private:
    /** The output stream to which data is to be written.

        This instance variable holds a reference to the output stream
        to which this class must dump all the XFig information.  The
        reference is initialized in the constructor and used by
        various methods in this class.
    */
    std::ofstream os;

    /** Instance variable to track if we are in polygon drawing mode.

        If this variable is not zero, then it indcates that this
        helper object is currently in ploygon drawing mode.  In
        polygon drawing mode the only valid method that can be invoked
        is addVertex() method.  This variable is set to number of
        vertex in startPolygon method and decremented each time
        addVertex() method is called.
    */
    int vertexCount;
};

#endif
