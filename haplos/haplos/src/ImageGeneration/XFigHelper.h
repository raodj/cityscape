#ifndef XFIG_HELPER_H
#define XFIG_HELPER_H

//-----------------------------------------------------------
//
// This file is part of HAPLOS availabe off the website at
// <http://pc2lab.cec.miamiOH.edu/haplos>
// 
// Human  Population  and   Location  Simulator (HAPLOS)  is
// free software: you can  redistribute it and/or  modify it
// under the terms of the GNU  General Public License  (GPL)
// as published  by  the   Free  Software Foundation, either
// version 3 (GPL v3), or  (at your option) a later version.
//
// HAPLOS is distributed in the hope that it will  be useful,
// but   WITHOUT  ANY  WARRANTY;  without  even  the IMPLIED
// WARRANTY of  MERCHANTABILITY  or FITNESS FOR A PARTICULAR
// PURPOSE.
// 
// Miami University and the HAPLOS  development team make no
// representations  or  warranties  about the suitability of
// the software,  either  express  or implied, including but
// not limited to the implied warranties of merchantability,
// fitness  for a  particular  purpose, or non-infringement.
// Miami  University and  its affiliates shall not be liable
// for any damages  suffered by the  licensee as a result of
// using, modifying,  or distributing  this software  or its
// derivatives.
//
// By using or  copying  this  Software,  Licensee  agree to
// abide  by the intellectual  property laws,  and all other
// applicable  laws of  the U.S.,  and the terms of the  GNU
// General  Public  License  (version 3).  You  should  have
// received a  copy of the  GNU General Public License along
// with HAPLOS.  If not, you may  download copies  of GPL V3
// from <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------

#include <iostream>
#include <fstream>
#include <exception>
#include <string>

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

// The XFIG default scale 1 inch == 1200 xfig units
#define XFIG_SCALE 1200

// The XFIG default pixel to fig units conversion
#define PIXEL_TO_XFIG_SCALE 15

// Text Justification
#define LEFT_JUSTIFIED   0
#define CENTER_JUSTIFIED 1
#define RIGHT_JUSTIFIED  2

// Fill style constants
#define SOLID_FILL 20
#define NO_FILL    -1

/** A convenience class to generate plain-text XFig entries for
    various components of an image.

    This class provides various method that can be used to generate
    different parts of an image into a given XFig file.
*/
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

        \return This method returns \c true if the file was opened and
        the header was successfully written.  On errors this method
        returns \c false.
        
        \param[in] width The width scale to be used for the figure.
        This value is typically 1200.

        \param[in] height THe height scale to be used for the figure.
        This value is typically just 2.
        
	\param[in] appendFlag If this parameter is set to \c true,
	then this method assumes that the supplied file name already
	has XFig data in it and rest of the information needs to be
	just appended to the file. Consequently, this method does not
	generate any headers in this case.

	\param[in] genCustomColors If this flag is set to true then
	this method adds an user-defined color table to the generated
	XFig file. The user-defined color table guarantees a set of
	visibly different colors for use in applications.        
    */
    bool setOutput(const std::string &fileName,
                   const int width = 100,
                   const int height = 100,
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

        \param[in] lineColor An optional color code to specify color
        of the line.

        \param[in] level The level/layer in the figure where line is
        to be generated.
    */
    void drawLine(const int x1, const int y1,
                  const int x2, const int y2,
                  const int lineColor = BLACK, const int level = 50);

    /** Dump code for displaying a text

        This method can be used to generate code for printing a text
        (or string) at a specific location.

	\param[in] text The line of text to be displayed in an image.

	\param[in] x The x-coordinate (in XFig units) where the text
	image is to be displayed.

	\param[in] y The y-coordinate (in XFig units) where the text
	image is to be displayed.	

	\param[in] justify The horizontal justification for the text.

	\param[in] colorCode The color code indicating the color for
	the text.

	\param[in] fontSize The font size. The default font size is
	12.

	\param[in] fontCode The code corresponding to the font. The
        default font is TimesNewRoman.

	\param[in] level The layer (or level) in the xfig file where
	the text is to be generated.
	
	\return Return height of text in xfig units!
    */
    int drawText(const std::string& text,
                 const int x, const int y,
		 const int justify   = LEFT_JUSTIFIED,
                 const int colorCode = BLACK,		 
                 const int fontSize  = 12,		 
		 const int fontCode  = 14,
                 const int level     = 50);

    /** Dump code for displaying a rectangle.

        This method can be used to generate a rectangle filled with a
        given color on the generated XFig file.

        \param[in] x The top-left x-coordinate for the rectangle (in
        XFig units).

        \param[in] y The top-left y-coordinate for the rectangle (in
        XFig units).        

        \param[in] width The width of the rectangle in XFig units.

        \param[in] height The height of the rectangle in XFig units.

        \param[in] lineColor The color code (0 to 32) for the color of
        the line/border for the rectangle.

        \param[in] fillColor The color code (0 to 32) for the color
        with which the rectangle is to be filled.

        \param[in] fillStyle Value (-1 to 20) to indicate if the
        rectangle should actually be filled.

        \param[in] level The logical layer/level at which the
        rectangle is to be generated.

        \param[in] thickness The thickness of line/border for the
        rectangle.
    */
    void drawRect(int x, int y, int width, int height, int lineColor = BLACK,
                  int fillColor = BLACK, int fillStyle = NO_FILL,
		  int level = 50, int thickness = 1);

    /** A convenience method to add custom colors and color codes to a
	given XFig file.

	\note All the necessary colors must be added to the XFig file
	before they are used to draw shapes.

	\param[in] colorCode The unique color code to be used in the
	XFig file.  Using duplicate color codes have undesired
	consequences.  This method does not check for duplicate color
	codes!

	\param[in] red The value for the red component of an RGB
	color. The value must be in the range 0 to 255 (inclusive).

	\param[in] green The value for the green component of an RGB
	color. The value must be in the range 0 to 255 (inclusive).

	\param[in] blue The value for the blue component of an RGB
	color. The value must be in the range 0 to 255 (inclusive).
    */
    void addColor(int colorCode, int red, int green, int blue);

    /** Start a polygon and add points via call to addVertex method.

        This method can be used to create a closed polygon consisting
        of a series of line segments.  Each line segment is specified
        by adjacent pairs of vertices.

        \param[in] vertexCount The number of vertices to be added to
        the polygon via call to the addVertex method.

        \param[in] lineColor The color code for the polygon's
        border/line.

        \param[in] fillColor The color for filling-in the polygon.

        \param[in] level The level at which the polygon is to be
        generated.

        \param[in] fillStyle The fill style for filling-in the
        polygon.

        \see addVertex
    */
    void startPolygon(const int vertexCount, const int lineColor = BLACK,
                      const int fillColor = RED, const int level = 50,
                      const int fillStyle = 20);

    /** Start a poly-line, that is a line with multiple vertices.

        This method can be used to create a poly-line with multiple
        line segments. Each line segment is specified by adjacent
        pairs of vertices.

        \param[in] vertexCount The number of vertices to be added to
        the polygon via call to the addVertex method.

        \param[in] lineColor The color code for the polygon's
        border/line.

        \param[in] level The level at which the polygon is to be
        generated.

        \see addVertex
    */    
    void startPolyLine(const int vertexCount, const int lineColor = BLACK,
		       const int level = 50);

    /** Add a vertex to current polygon or poly-line.

        This method must be used to add the next vertex in a polygon
        or poly-line.  Note that vertices must be added in correct
        order for the polygon/poly-line to be rendered correctly.
        Needless to add a suitable call to startPolygon or
        startPolyLine must have been already called.

        \param[in] x The x-coordinate of the vertex.

        \param[in] y The y-coordinate of the vertex.
    */
    void addVertex(const int x, const int y);

    /** Indicate end of polygon generation.

        THis method is the dual of startPolygon method.  This method
        must be invoked to let the generator know that all the
        vertices in a polygon have been added.  This method
        essentially ensures that the promised number of vertices were
        successfully added.
    */
    void endPolygon();

    /** Indicate end of poly-line generation.

        THis method is the dual of startPolyLine method.  This method
        must be invoked to let the generator know that all the
        vertices in a poly-line have been added.  This method
        essentially ensures that the promised number of vertices were
        successfully added.
    */
    void endPolyLine();

    /** Dump code for displaying a image in the figure.

        This method can be used to embed an image in the generated
        XFig file.

        \param[in] fileName Path to the image file to be embedded into
        the figure.
        
        \param[in] x The top-left x-coordinate for the image (in
        XFig units).

        \param[in] y The top-left y-coordinate for the image (in
        XFig units).        

        \param[in] width The width of the image in XFig units.

        \param[in] height The height of the image in XFig units.

        \param[in] layer The logical layer/level at which the
        rectangle is to be generated.
    */
    void addImage(const std::string& fileName, const int x, const int y,
                  const int width, const int height, const int layer = 60);

    /** Dump code for displaying a oval in the figure.

        This method can be used to generate a hollow or filled oval
        with different x and y radii.

        \param[in] x The center x-coordinate for the oval (in
        XFig units).

        \param[in] y The center y-coordinate for the oval (in XFig
        units).

        \param[in] xRad The x-radius for the oval in XFig units.

        \param[in] yRad The y-radius for the oval in XFig units.        

        \param[in] lineColor The color code (0 to 32) for the color of
        the line/border for the rectangle.

        \param[in] fillColor The color code (0 to 32) for the color
        with which the rectangle is to be filled.

        \param[in] fillStyle Value (-1 to 20) to indicate if the
        rectangle should actually be filled.

        \param[in] level The logical layer/level at which the
        rectangle is to be generated.
    */
    void drawOval(const int x, const int y,
                  const int xRad, const int yRad,
		  const int lineColor = BLACK,
		  const int fillColor = RED, const int level = 50,
		  const int fillStyle = 20);

    /** Add a comment to annotate the next entity drawn.

        This method is used to add a simple comment to provide
        metadata or annotation about the next entity to be drawn.

        \param[in] comment The comment string to be included in the
        figure.
    */
    void addComment(const std::string& comment);
    
protected:
    /** Method to dump initial XFig header to the file.
        
        This is a helper method that must be used to dump a standard
        XFig header to the supplied output file.
        
        \param[in] width The width scale to be used for the figure.
        This value is typically 1200.

        \param[in] height The height scale to be used for the figure.
        This value is typically just 2.
        
	\param[in] genCustomColors If this flag is set to \c true then
	this method adds an user-defined color table to generated
	XFig. The user-defined color table guarantees a set of visibly
	different colors for use in applications.
    */
    void dumpHeader(const bool genCustomColors, const int width = 1200,
                    const int height = 2);

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
