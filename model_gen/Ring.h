#ifndef RING_H
#define RING_H

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

#include <utility>
#include <vector>
#include <string>
#include "XFigHelper.h"

/** A convenience alias to store coordinates for vertices.
    
    Point is conveniene alias to refer to the x-coordiante or
    longitude (in Point::first) and the y-coordiante or latitude
    (Point::second) values associated with a given vertex.
*/
using Point = std::pair<double, double>;

/** A class to encapsulate information about a Ring in a shape file.

    A shape file essentially consists of a collection of polygons that
    are called rings.  The default shape file consists of multiple
    shapes with rings embedded inside shapes. The default organization
    of a shape file exacerbates the tasks of identifying, accessing,
    and processing rings. Consequently, this class has been introduced
    to streamline the process of accessing and processing rings in a
    shape file.
*/
class Ring {
    // Friend declaration to setup protected properties
    friend class ShapeFile;
    friend std::ostream& operator<<(std::ostream&, const Ring&);
public:
    /** The different types of rings that are typically loaded from a
        shape file.

        These enumerations provide a way to distinguish the different
        types of shapes that can be loaded from a given shape file.
    */
    enum Kind {UNKNOWN_RING, CLOSED_RING, ARC_RING, POPULATION_RING,
               BUILDING_RING, ENTRY_RING};

    /** A convenience wrapper sub-class to hold a metadata about the
        ring.  These entries are typically loaded from a separate DBF
        file.  The information is maintained in the ring to ease
        various processing.
    */
    class Info {
    public:
        int type;          // 0: string, 1: int, 2: double, 3: bool, 4: err
        std::string name;  // name associated with the field
        std::string value; // Value stored as string for convenience.
    };
    
    /** The primary constructor.

        The constructor initializes the Ring object with the supplied
        parameters.

        \param[in] ringID A unique identifier to refer to this
        ring. This number is primarily useful for debugging purposes.
        
        \param[in] shapeID The zero-based ID value associated with
        this shape.  This shapeID corresponds to the shape in the
        shape file that actually contains this ring.
        
        \param[in] vertexCount The number of vertices that constitute
        this ring.  This value must be at least 4 (for a triangular
        shape).

        \param[in] xCoords The array of values that represent the
        x-coordiantes of the vertices assocaited with this Ring. This
        class duplicates the data in its own internal pointers.

        \param[in] yCoords The array of values that represent the
        y-coordiantes of the vertices assocaited with this Ring. This
        class duplicates the data in its own internal pointers.

        \param[in] status An optional status string to be associated
        with this Ring.  The status string information is typically
        read from a DBase database file associated with a given shape
        file.  The data is loaded when a shape file is loaded.

	\param[in] scale An optional scale value by which points are
	to be scaled. This is a convenience value that can be used to
	scale rings or adjust values from disparate sources to be
	consistent.  Each value is divided by the scale factor.
    */
    Ring(const long ringID, const long shapeID, const Ring::Kind kind,
         const int vertexCount, const double *xCoords, const double *yCoords,
         const std::vector<Ring::Info>& infoList, double scale = 1.0);

    /** Copy constructor to duplicate a ring object.

	The copy constructor can be used to create a duplicate
	(possibly mutable) copy of a given ring. Note that the newly
	created ring will \b not be added to the shape file.

	\param[in] source The source ring from where the data is to be
	copied (or duplicated).
    */
    Ring(const Ring& source);

    /** Create a ring with a subset of vertices from another ring.

        This constructor is similar to the copy constructor, except
        that the ring is created with a subset of vertices from the
        source ring.

	\param[in] source The source ring from where the data is to be
	copied (or duplicated).

        \param[in] startIndex The starting index for the vector of
        points for the new ring.

        \param[in] startIndex The ending index for the vector of
        points for the new ring.
    */
    Ring(const Ring& source, const int startIndex, const int endIndex);

    /**
     * Convenience constructor to make a new Ring to represent a
     * square population grid.
     *
     * \param[in] topLeft The top-left coordinate for this rectangular
     * ring.
     *
     * \param[in] botRight The bottom-right coordinate for this
     * rectangular ring.
     *
     * \param[in] population The population value for this ring.
     *
     * \param[in] ringID An optional ID for this ring.
     *
     * \param[in] shapeID An option ID for the shape associated with
     * this ring.
     */
    Ring(const Point& topLeft, const Point& botRight,
         const double population, const long ringID = -1,
         const long shapeID = -1, const std::vector<Ring::Info>& infoList = {});

    /** Very specialized method to create a rectangular ring.

        This is a very specialized method used in this application --
        given 2 points on a line (typically start and end points of a
        trip), this method creates a rectangular ring that is "dist"
        miles around the two points (parallel to the lines) as shown
        in the figure below:

        <pre>
                  +----------------------+
                  |                      |
                  |  S==============>E   |
                  |                      |
                  +----------------------+
       </pre>

       \note This method is not very accurate in the buffer zone
       created around the points, particularly for vertical lines (it
       underestimates the buffer zone).  However, for small values
       this method is acceptable.
       
       \param[in] start The first point on the line. Typically this is
       the starting point of a trip.

       \param[in] end The ending point on the line. Typically this is
       the ending point of a trip.

       \param[in] dist The distance in miles around the points that the
       rectangle should encompass.  The default distance is 1 mile.

       \param[in] population Optional population value for this ring.

       \param[in] ringID An optional ID for this ring.

       \param[in] shapeID An option ID for the shape associated with
       this ring.

       \param[in] infoList Optional information associated with this
       ring.
    */
    static Ring createRectRing(const Point& start, const Point& end,
                               const double dist = 1.0,
                               const double population = 0,
                               const long ringID = -1, const long shapeID = -1,
                               const std::vector<Ring::Info>& infoList = {});
    
    /** Obtain the shape ID value for this Ring.

        The shapeID value corresponds to the shape in the shape file
        that actually contains this ring. Rings that are part of the
        same shape will have the same shapeID value.
        
        \return The zero-based shape-ID (or shape index) value for
        this Ring.
    */
    inline long getShapeID() const { return shapeID; }

    /** Obtain the ring ID value for this Ring.

        The ringID value corresponds to the ring.
        
        \return The zero-based ring-ID (or ring index) value for
        this Ring.
    */
    inline long getRingID() const { return ringID; }

    /** Obtain number of vertices in the Ring.

        This method can be used to determine the number of vertices in
        this Ring. This method essentially returns the value set when
        this Ring was instantiated.
        
        \return The number of vertices in the ring.
    */
    inline int getVertexCount() const { return vertexCount; }

    /** Obtain a std::pair object containing vertex coordinates.

        This method provides a convenient interface to obtain the
        x-coordiante (longitude) and the y-coordiante (latitude)
        values associated with a given vertex.  This method returns a
        Point object (Point is typedef for std::pair<double, double>)
        \p where \p.first is x-coordinate and \p.second is the
        y-coordinate.

        \param[in] index The index of the vertex in this Ring. This
        value must be in the range \c 0 to \c vertexCount-1.

        \return A point object containing the x and y coordinates for
        the given vertex.  If the index is invalid then this method
        returns <nan, nan> (nan: not a number) as the value.
    */
    Point getVertex(const int index) const;

    /** Change the coordinates of a given vertex in this ring.

        This method can be used to change the coordinates assocaited
        with a given vertex.

        \param[in] index The zero-based index position of the vertex
        whose coordinates are to be changed.

        \param[in] value The new coordinates to be set for the vertex.
        Point is an alias for std::pair<double, double>, where
        point.first is x-coordinate (longitude) and point.second is
        the y-coordinate (latitude).
    */
    void setVertex(const int index, const Point& value);
    
    /** Determine if this Ring must be exlcuded from the shape area.

        This method may be used to determine if this Ring must be
        excluded from the shape area.

        \return This method returns \c true if the Ring must be
        excluded from the shape area.
    */
    inline bool isSubtraction() const { return subtractFlag; }

    /** Determine if a given point lies inside this Ring.

        This method may be used to determine if a given point
        (represented by xCoord and yCoord) lies within the given Ring.

        \param[in] xCoord The x-coordinate value (or the longitude) of
        the point to be tested.

        \param[in] yCoord The y-coordinate value (or the latitude) of
        the point to be tested.
        
        \return This method returns \c true if the point lies within
        the Ring. Otherwise it returns \c false.
    */
    bool contains(const double xCoord, const double yCoord) const;

    /** Determines if the given point is close to one of the edges of
        this ring.  The determiniation if the point is close is done
        by dropping a perpendicular from the point to the edge and
        checking if the distance is less than the threshold specified.

        \param[in] xCoord The x-coordinate value (or the longitude) of
        the point to be tested.

        \param[in] yCoord The y-coordinate value (or the latitude) of
        the point to be tested.

        \param[in] maxDist The maximum acceptable distance (in miles)
        between the point and this ring.

        \return This method returns \c true
        if the point lies within the Ring. Otherwise it returns \c
        false.
     */
    bool isNear(const double xCoord, const double yCoord,
                const double maxDist = 0.2) const;
    
    /** Determine if a given Ring is completely contained by this
        Ring.

        This method may be used to test if another Ring is completely
        contained by this ring.  This method essentially checks if all
        the points of the supplied ring (parameter) are contained
        within this ring.  If so, this method returns \c true. Even if
        one of the points is not contained within this ring then this
        method returns \c false.

        \param[in] ring The ring to be tested for containment.

        \return This method returns \c true if the ring (parameter)
        completely lies within this ring.  If the ring (parameter) is
        the same as \c this, then this method returns \c true.
    */
    bool contains(const Ring& ring) const;

    /** Obtain area of this Ring.

	This method uses the shoelace method to compute the area of
	the irregular ring.
	
	\return The area of this Ring in square miles.
    */
    double getArea() const;

    /** Obtain bounds for this ring.

	This method can be used to obtain the rectangular bounds for
	the ring.

	\param[out] topLeft The point representing the top-left corner
	of the bounding rectangle. Note that this point may not be an
	actual vertex in the Ring.

	\param[out] botRight The point representing the bottom-right
	corner of the bounding rectangle. Note that this point may not
	be an actual vertex in the Ring.
    */
    void getBounds(Point& topLeft, Point& botRight) const;

    /** Get the center point for this polygon.

	This method can be used to determine the center of this ring.

        \note The centroid is not always inside of the polygon.
        
	\return A Point object containg the x (in point.first) and y
	(in point.second) coordinate of the center of this polygon.
    */
    Point getCentroid() const;

    /** Get the Nearest Point in this Ring to the specified point.

        This method iterates through all of the points in this Ring to
        determine which point is the closest one to the coordinates
        specified.  If multiple points are equally close, the first
        point found is returned.

	\param[in] refPoint The reference point to which the index of
	the closest vertex is desired.  It is assumed that the
	latitude/x coordinate is stored in refPoint.first and the
	longitude/y coordinate is stored in refPoint.second.
	
        \return A Point object containing the x and y coordinate of
        the nearest point in this polygon.
    */
    int getNearestIndex(const Point& refPoint) const;

    /** Get the index of teh nearest vertex in this Ring to a
	specified point.

        This method iterates through all of the points in this Ring to
        determine the vertex that is the closest one to a given
        reference point (parameter).  If multiple vertices are equally
        close, the first vertex encountered is chosen.

	\param[in] refPoint The reference point to which the index of
	the closest vertex is desired.  It is assumed that the
	latitude/x coordinate is stored in refPoint.first and the
	longitude/y coordinate is stored in refPoint.second.
	
        \return The index of a vertex that is closest to a given
        reference point.
    */
    Point getNearestPointTo(const Point& refPoint) const;
    
    /** The type of information represented by this Ring.

        This method can be used to determine the type of information
        represented by this Ring.  This information is set when this
        Ring was loaded from a shape file.

        \return The kind of information represented by this Ring.
    */
    inline Ring::Kind getKind() const { return kind; }

    /** Rotate a shape around a given point.

	This method can be used to rotate the ring around around a
	given point.

	\param[in] center The center point around which the shape must
	be rotated.

	\param[in] angle The angle in radians by which the shape must
	be rotated. The angle must be in radians.
    */
    void rotate(const Point& center, const double angle);

    /** Translate a shape by a given offset value.

	This method can be used to translate this ring by a given
	offset value.

	\param[in] offset The longitude/x (offset.first) and
	latitude/y (offset.second) offset values that must be added to
	each point in this Ring.  These offset values can be positive
	or negative.
    */
    void translate(const Point& offset);

    /** Determine if this ring is all in the southern hemisphere.

	This method can be used to determine if the ring is all in the
	southern hemishpere. More specifically, it can also be used to
	determine if points in this Ring is sufficiently south of the
	equator by specifiying a \b negative latitude as the
	parameter. For example isSouthernRing(-10) will return true
	only if all the verticies in his ring are south of -10 degree
	latitude.
	
	\return This method returns true if all the vertices are in
	the southern hemisphere.
    */
    bool isSouthernRing(const double northernMostLat = 0.0);
    
    /** The destructor.

        The destructor has been made protected to ensure that Ring
        objects are only deleted by the ShapeFile class that owns this
        Ring. The destructor frees up the memory used to hold the x
        and y coordinates for this ring.
    */
    virtual ~Ring();

    /** Convenience method to print information about this Ring in an
        XFig format.

        \param[out] os The output stream to where the XFig data is to
        be printed.

        \param[in] figSize The overall size of the xfig file.

        \param[in] xClip The left-most value to which the locations
        are to be printed -- i.e. left-most latitude at as
        x-coordinate zero in the generated xfig.

        \param[in] yClip The top-most value to which the locations are
        to be printed -- i.e. top-most longitude at as y-coordinate
        zero in the generated xfig.

        \param[in] drawCentroid Draw a cricle at the centroid of the
        ring.

        \param[in] fillColor The fillColor to be used to draw the ring.

        \param[in] layer The XFig layer at which the ring is to be
        printed. Note that the layer is decreased by 2 if this ring is
        a subtraction ring so that subtraction rings automatically
        show up above other rings.
    */
    void printXFig(XFigHelper& fig, const int figSize, const int xClip = 0,
                   const int yClip = 0, const bool drawCentroid = false,
                   const int fillColor = -1, int layer = 50) const;

    /** Returns the information associated with a given col name.
     *
     * \param[in] colName The column for which the information is to
     * be returned.
     *
     * \return The value associated with this column.
     */
    std::string getInfo(const std::string& colName) const;

    /** Returns all of the metadata associated with this ring as a
     * series of name=value pairs.
     *
     * \return The list of metadata associated with this column.
     */
    std::string getInfo() const;

    /** Returns the information associated with a given set of columns.
     *
     * \param[in] colNames The list of column names for which the
     * information is to be returned.
     *
     * \return The value associated with the given list of columns.
     */
    std::string getInfo(const std::vector<std::string>& colNames) const;
    
    /** Get the label that was set earlier for this ring.
     *
     * \return The label associated with this ring.
     */
    std::string getLabel() const { return label; }

    /**
     * Set the label for this ring.
     *
     * \param[in] str The label to be set for this label.
     */
    void setLabel(const std::string& str) { label = str; }

    /** Obtain the population value set for this ring.

        \return The population for this ring.
     */
    double getPopulation() const { return population; }
    
protected:
    /** Set the subtraction flag to indicate this ring is an \b
        exclusion.

        This method is used by the ShapeFile to detect and set if this
        Ring represents an area that must be excluded or subtracted
        from another ring.

        \param[in] subtract If this parameter is \c true that
        indicates that this ring must be subtracted from the set of
        rings constituting the shape.
    */
    void setSubtractionFlag(const bool subtract);
   
private:
  /** The zero-based ring ID value for this shape.
        
        The zero-based ID value associated with this shape.  This ID
        has no specific corresponding to any physically stored
        data. However, it is useful for debugging purposes.
    */
    long ringID;
    
    /** The zero-based shape ID value for this shape.
        
        The zero-based ID value associated with this shape.  This
        shapeID corresponds to the shape in the shape file that
        actually contains this ring.
    */
    long shapeID;
    
    /** The number of vertices constituting this ring.

        This instance variable is used to track the number of vertices
        constituting this ring. This value is initialized in the
        constructor and is never changed during the life time of this
        object.
    */
    int vertexCount;

    /** The x-coordinate values for each vertex.

        This instance variable holds a pointer to the list of
        x-coordinate values for the vertices constituting this
        Ring.  Values are set when a ring is instantiated.
    */
    std::vector<double> xCoords;

    /** The y-coordinate values for each vertex.
        
        This instance variable holds a pointer to the list of
        y-coordinate values for the vertices constituting this
        Ring. Values are set when a Ring is instantiated.
    */
    std::vector<double> yCoords;

    /** Optional informational items associated with this ring.

        This vector maintains a list of informational items associated
        with this ring.  The information is typically loaded from a
        corresponding but separate DBF file.  The information is
        maintained in the Ring to provide convenient access to the
        metadata for various operations.
    */
    std::vector<Ring::Info> infoList;
    
    /** The status string associated with this ring.

        This instance variable holds the status information string
        assocaited with this ring.  The status information is
        optionally loaded from a DBase data base file associated with
        a given shapefile.  The set of valid status string information
        include: \c "breeding", \c "feeding, wintering", \c "all year
        round", \c "feeding".  If a status value is not available,
        then this string is set to an empty string (\c "").
    */
    std::string status;

    /** Set a convenience label for this ring */
    std::string label;
    
    /** Flag to indicate if this ring must be excluded from the area.

        This instance variable is initialized to \c false in the
        constructor and is later on changed (if needed) by the
        setSubtractionFlag method.  If this instance variable is \c
        true, then this Ring must be excluded from the area
        represented by the shape (which logically contains this ring).
    */
    bool subtractFlag;

    /** The type of information represented by this ring.

        The kind identifies the type of information represented by
        this ring. It can either be a closed polygon (CLOSED_RING) or
        an open arc (ARC_RING).
    */
    Kind kind;

    /** The population (if any) associated with a population ring */
    double population;
};

#endif
