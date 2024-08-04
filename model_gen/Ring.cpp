#ifndef RING_CPP
#define RING_CPP

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

#include <cmath>
#include <iostream>
#include <set>
#include "Ring.h"
#include "Utilities.h"

// We use BOOST geometry for intersecting Rings. Note that we do boost
// includes in the cpp file to keep the compilation fast -- If we
// include boost::geometry in the header it triples the compile times.
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

Ring::Ring() : ringID(-1), shapeID(-1), vertexCount(0),
               kind(Ring::UNKNOWN_RING) {
    // Nothing else to be done for now.
    // Copy other instance variables.
    subtractFlag = false;
    population   = -1;
}

Ring::Ring(const long ringIdx, const long shapeIdx, const Ring::Kind type,
           const int vCount, const double *x, const double *y,
           const std::vector<Ring::Info>& infoList, const double scale)
    : ringID(ringIdx), shapeID(shapeIdx), vertexCount(vCount),
      xCoords(x, x + vertexCount), yCoords(y, y + vertexCount),
      infoList(infoList), kind(type) {
    // Set other instance variables to default values
    subtractFlag = false;
    // Scale the coordinates (if scale is not 1.0)
    if (scale != 1.0) {
	for(int i = 0; (i < vertexCount); i++) {
	    xCoords[i] /= scale;
	    yCoords[i] /= scale;
	}
    }
    // Initialize population to zero
    population = 0;
}

Ring::Ring(const Ring& source)
    : ringID(source.ringID), shapeID(source.shapeID),
      vertexCount(source.vertexCount), xCoords(source.xCoords),
      yCoords(source.yCoords), infoList(source.infoList), status(source.status),
      label(source.label), kind(source.kind) {
    // Copy other instance variables.
    subtractFlag = source.subtractFlag;
    population   = source.population;
}

Ring::Ring(const Ring& source, const int startIndex, const int endIndex) :
    ringID(source.ringID), shapeID(source.shapeID),
    vertexCount(endIndex - startIndex), status(source.status),
    label(source.label), kind(source.kind) {
    ASSERT ( endIndex <= source.vertexCount );
    ASSERT ( startIndex >= 0 );
    ASSERT ( vertexCount >= 0 );
    subtractFlag = source.subtractFlag;
    population   = source.population;
    // Copy the subset of vertices
    xCoords = std::vector<double>(source.xCoords.begin() + startIndex,
                                  source.xCoords.begin() + endIndex);
    yCoords = std::vector<double>(source.yCoords.begin() + startIndex,
                                  source.yCoords.begin() + endIndex);
}

Ring::Ring(const Point& topLeft, const Point& botRight,
           const double pop, const long ringID, const long shapeID,
           const std::vector<Ring::Info>& infoList) :
    ringID(ringID), shapeID(shapeID), infoList(infoList),
    kind(POPULATION_RING)  {
    // Setup the x & coordinates to make a polygon by setting up
    // coordinates in a clockwise manner starting with top-left. 
    vertexCount  = 5;   
    xCoords ={topLeft.first,  botRight.first, botRight.first,  topLeft.first, topLeft.first};
    yCoords ={topLeft.second, topLeft.second, botRight.second, botRight.second, topLeft.second};
    // Setup other properties
    population   = pop;
    subtractFlag = false;
}

Ring::~Ring() {
    // Nothing much to do
}

Point
Ring::getVertex(const int index) const {
    if ((index < 0) || (index >= vertexCount)) {
        return Point(NAN, NAN);
    }

    return Point(xCoords[index], yCoords[index]);
}

void
Ring::setVertex(const int index, const Point& value) {
    if ((index < 0) || (index >= vertexCount)) {
        return;
    }
    xCoords[index] = value.first;
    yCoords[index] = value.second;
}

bool
Ring::contains(const double x, const double y) const {
    bool result = false;
    for (int vertex = 0, nextVert = vertexCount-1; (vertex < vertexCount);
         nextVert = vertex++) {
        if ((((yCoords[vertex]   <= y) && (y < yCoords[nextVert])) ||
             ((yCoords[nextVert] <= y) && (y < yCoords[vertex]))) &&
            (x < (xCoords[nextVert] - xCoords[vertex]) * (y - yCoords[vertex]) / (yCoords[nextVert] - yCoords[vertex]) + xCoords[vertex])) {
            // Toggle result tracking to see if this check successed
            // an odd number of times.  If the check succeeds an odd
            // number of times then the point lies within this Ring.
            result = !result;
        }
    }
    
    return result;
}

bool
Ring::contains(const Ring& ring) const {
    const int VertexCount = ring.getVertexCount();
    for(int vertex = 0; (vertex < VertexCount); vertex++) {
        Point p = ring.getVertex(vertex);
        if (!contains(p.first, p.second)) {
            // This vertex is not within this ring. Therefore the
            // parameter ring is not contained.
            return false;
        }
    }
    // When control drops here that means all the vertices of the
    // parameter ring are contained by this ring.
    return true;
}

void
Ring::setSubtractionFlag(const bool subtract) {
    subtractFlag = subtract;
}

double
Ring::getArea() const {
    // Use the shoelace approach to compute the area of this
    // irregular polygon.
    double leftToRight = 0;
    double rightToLeft = 0;
    const  double MilesPerLat = 69.047;
    for (int i = 0; (i < vertexCount - 1); i++) {
        // Convert first point to relative miles offset from equator
        double x1 = xCoords[i] * MilesPerLat * cos(TO_RADIANS(yCoords[i]));
        double y1 = yCoords[i] * MilesPerLat;
        // Convert second point to relative miles offset from equator
        double x2 = xCoords[i+1] * MilesPerLat * cos(TO_RADIANS(yCoords[i+1]));
        double y2 = yCoords[i+1] * MilesPerLat;
        // Now do the actual shoelace operations
        leftToRight += x1 * y2;
        rightToLeft += y1 * x2;
    }
    return 0.5 * fabs(leftToRight - rightToLeft);
}

void
Ring::getBounds(Point& topLeft, Point& botRight) const {
    // Initialize to starting value.
    topLeft = botRight = Point(xCoords[0], yCoords[0]);
    // Search rest of the vertices for smaller/larger values
    for(int i = 1; (i < vertexCount); i++) {
        // determine top-left by finding minimum x & y coordinates
        topLeft.first  = std::min<double>(topLeft.first,  xCoords[i]);
        topLeft.second = std::min<double>(topLeft.second, yCoords[i]);
        // determine bottom-right by finding maximum x & y coordinates
        botRight.first  = std::max<double>(botRight.first,  xCoords[i]);
        botRight.second = std::max<double>(botRight.second, yCoords[i]);
    }
}

Point
Ring::getCentroid() const {
    double cx = 0, cy = 0;
    // Computing centroid for small polygons introduces considerable
    // error due to smear in precision.  So we subtract the 1st point
    // to offset the values so as to avoid smear and get correct
    // values.
    const double xOff = xCoords[0], yOff = yCoords[0];
    // Use the shoelace approach to compute the area of this
    // irregular polygon.
    double area = 0;
    for(int i = 0; (i < (vertexCount - 1)); i++) {
        // Compute offset values for x and y coordinates to avoid
        // smear issues with precision
        const double xi  = (xCoords[i]   - xOff);
        const double yi  = (yCoords[i]   - yOff);
        const double xi1 = (xCoords[i+1] - xOff);
        const double yi1 = (yCoords[i+1] - yOff);
        //Compute common term to shave off cycles.
        const double common = (xi * yi1) - (xi1 * yi);
        // Track net cetroid
        cx += (xi + xi1) * common;
        cy += (yi + yi1) * common;
        // Do the shoelace operations to compute area.
        area += common;
    }
    // Divide values by 3*area as per formula.
    const double divisor = 3.0 * area;
    // Return the centroid
    return Point(xOff + cx / divisor, yOff + cy / divisor);
}

Point
Ring::getNearestPointTo(const Point& refPoint) const {
    // Use other method to obtian index of nearest point.
    int indexNearest = getNearestIndex(refPoint);
    // Return coordinates of nearest vertex as a point object
    return Point(xCoords[indexNearest], yCoords[indexNearest]);
}

int
Ring::getNearestIndex(const Point& refPoint) const {
    // Assume first vertex is the nearest and note its distance
    int indexNearest = 0;
    double nearestDist = hypot(xCoords[0] - refPoint.first,
                               yCoords[0] - refPoint.second);
    // Valdiate initial assumption against all other verices to locate
    // a possibly closer vertex than before.
    for (int i = 1; (i < vertexCount); i++) {
        const double tempNearestDist =  hypot(xCoords[i] - refPoint.first,
                                              yCoords[i] - refPoint.second);
        if (tempNearestDist < nearestDist) {
            nearestDist = tempNearestDist;
            indexNearest = i;
        }
    }
    // Return the index of the closest vertex in this ring.
    return indexNearest;
}

void
Ring::rotate(const Point& center, const double angle) {
    // Precompute constant values
    const double cosAng = cos(angle);
    const double sinAng = sin(angle);
    for(int vert = 0; (vert < vertexCount); vert++) {
        // Compute common dX and dY for rotation
        const double deltaX = xCoords[vert] - center.first;
        const double deltaY = yCoords[vert] - center.second;
        // Compute the revised coordinates
    	xCoords[vert] = center.first  + (deltaX * cosAng - deltaY * sinAng);
        yCoords[vert] = center.second + (deltaX * sinAng + deltaY * cosAng);
    }
}

void
Ring::translate(const Point& offset) {
    // Translate each vertex.
    for(int vert = 0; (vert < vertexCount); vert++) {
        // Compute the revised coordinates
    	xCoords[vert] += offset.first;
        yCoords[vert] += offset.second;
    }
}

bool
Ring::isSouthernRing(const double northernMostLat) {
    for(int vert = 0; (vert < vertexCount); vert++) {
        if (yCoords[vert] > northernMostLat) {
            // There is at least one vertex north of northernMostLat
            return false;
        }
    }
    // All vertices are south of northernMostLat
    return true;
}

std::string
Ring::getInfo() const {
    std::string retVal;
    for (const Ring::Info& info : infoList) {
        retVal += info.name + "='" + info.value + "' ";
    }
    return retVal;
}

std::string
Ring::getInfo(const std::string& colName) const {
    for (const Ring::Info& info : infoList) {
        if (info.name == colName) {
            return info.value;
        }
    }
    return "";
}

std::string
Ring::getInfo(const std::vector<std::string>& colName) const {
    std::string retVal;
    for (const std::string& str : colName) {
        retVal += getInfo(str) + " ";
    }
    // Remove trailing blank space.
    retVal.pop_back();
    return retVal;
}

#ifndef NO_XFIG
void
Ring::printXFig(XFigHelper& xfig, const int figSize, const int xClip,
                const int yClip, const bool drawCentroid,
                const int fillColor, int layer, int lineColor) const {
    // Add a comment to the xfig file for this ring.
    std::string info = getInfo();
    if (!info.empty()) {
        xfig.addComment(info);
    }
    if (getKind() == Ring::CLOSED_RING) {
        if (isSubtraction()) {
            // Put subtraction rings at a higher layer.
            layer -= 2;
        }        
        // Start polygon
        const int fillStyle = (fillColor != -1 ? SOLID_FILL : NO_FILL);
        xfig.startPolygon(getVertexCount(), lineColor, fillColor, layer,
                          (isSubtraction() ? 41 : fillStyle));
    } else if ((getKind() == Ring::POPULATION_RING) ||
               (getKind() == Ring::BUILDING_RING)   ||
               (getKind() == Ring::SYNTH_BUILDING_RING)   ||
               (getKind() == Ring::PUMA_RING)) {
        // Start polygon with rings of different kinds on different layers
        // to ease viewing them separately in xfig program.
        const int polyLyr = layer + 2 + getKind();
        xfig.startPolygon(getVertexCount(), lineColor, fillColor, polyLyr,
                          SOLID_FILL);
    } else {
        // This ring is an ARC so draw it as a series of lines instead.
        xfig.addComment("Arc " + std::to_string(ringID));
        // xfig.startPolyLine(ring.getVertexCount(), ringIdx % 7, 20);
        const int level = (getKind() == Ring::ARC_RING ? 20 : 25);
        const int color = (getKind() == Ring::ARC_RING ? lineColor : BLUE);
        xfig.startPolyLine(getVertexCount(), color, level);
    }
    
    // Add all the vertices.
    for (int vertex = 0;  (vertex < getVertexCount()); vertex++) {
        int xfigX, xfigY;
        Point p = getVertex(vertex);
        getXYValues(p.second, p.first, xfigX, xfigY,
                    figSize * XFIG_SCALE, false);
        xfigX -= xClip;
        xfigY -= yClip;
        xfig.addVertex(xfigX, xfigY);
    }
    // Done with one ring and on to the next one.
    if ((getKind() != Ring::ARC_RING) && (getKind() != Ring::ENTRY_RING)) {
        xfig.endPolygon();
    } else {
        xfig.endPolyLine();
    }
    
    // Draw the centroid of each closed polygon ring if requested and
    // it is not a subtraction region
    if (drawCentroid && !isSubtraction() &&
        (getKind() != Ring::ARC_RING) && (getKind() != Ring::ENTRY_RING) &&
        (getKind() != Ring::POPULATION_RING)) {
        Point center = getCentroid();
        int xfigX, xfigY;
        getXYValues(center.second, center.first, xfigX, xfigY,
                    figSize * XFIG_SCALE, false);
        xfigX -= xClip;
        xfigY -= yClip;
        xfig.addComment("Centroid for: " + info);
        xfig.drawOval(xfigX, xfigY, 30, 30, MAGENTA, MAGENTA, 30);
        // Also, print a label at the Centroid if specified.
        if ((ringID != -1) && !label.empty()) {
            xfigY += (XFIG_SCALE * 6);
            xfig.drawText(label, xfigX, xfigY, CENTER_JUSTIFIED, 4, 10, 0, 28);
        }
    }
}
#endif

std::ostream& operator<<(std::ostream& os, const Ring& ring) {
    // Print general information about the ring.
    os << "Ring(ID: " << ring.ringID << ", shapeID: " << ring.shapeID
       << ", kind: "  << ring.kind   << "):";
    // Print coordinates for vertices in the ring.
    for (int i = 0; (i < ring.vertexCount); i++) {
        os << " <" << ring.xCoords[i] << ", " << ring.yCoords[i] << ">";
    }
    // Print centroid for the ring.
    const Point centroid = ring.getCentroid();
    os << ", centroid: " << centroid.first << ", " << centroid.second;
    return os;
}

bool
Ring::isNear(const double xCoord, const double yCoord,
             const double maxDist) const {
    // Track the minimum perpendicular distance from the given point
    // to each line-segment of this ring.
    double minPerpDist = 1000;
    // Track perpendicular distance from each segment
    const int vertCount = getVertexCount() - 1;
    for (int i = 0; (i < vertCount); i++) {
        // Find perpendicular intercept from fixed point to this segement
        double interLat, interLon;
        if (findPerpendicularIntersection(yCoord, xCoord, yCoords[i],
                                          xCoords[i], yCoords[i + 1],
                                          xCoords[i + 1], interLat, interLon)) {
            // Found a valid perpendicular intercept. Track distance
            const double dist = getDistance(yCoord, xCoord, interLat, interLon);
            minPerpDist       = std::min(minPerpDist, dist);
        }
    }
    // The point is near this ring if the minimum perpendicular
    // distance is below the maxDist threshold specified.
    return (minPerpDist <= maxDist);
}

// Static method return a rectangular ring around 2 points.
Ring
Ring::createRectRing(const Point& node1, const Point& node2,
                     const double dist, const double population,
                     const long ringID, const long shapeID,
                     const std::vector<Ring::Info>& infoList) {
    // Anonymous helper method to rotate a point by 90-degrees. This
    // method is not accurate, particularly for vertical lines because
    // of lat/lon differences -- however, for small distances it is
    // sufficiently approximate.
    auto rotated = [](const Point& p1, const Point& p2, const double sign) {
        return Point(p1.first  + (p1.second - p2.second) * sign,
                     p1.second - (p1.first  - p2.first)  * sign);
    };
    
    // Find points that are given distance away from node1 and
    // node2. These are the two distance points we will be using to
    // compute coordinates for the rectangle.
    Point ext1, ext2;
    getPoint(node1.second, node1.first, node2.second, node2.first, -dist,
             ext1.second, ext1.first);
    getPoint(node2.second, node2.first, node1.second, node1.first, -dist,
             ext2.second, ext2.first);
    // Now, compute rotated points based on ext1, node1 and ext2, node2
    const std::vector<Point> rect = { rotated(ext1, node1, 1),
                                      rotated(ext1, node1, -1),
                                      rotated(ext2, node2, 1),
                                      rotated(ext2, node2, -1) };
    // Now convert the points to flat x and y coordinates so that we
    // can use the Ring's constructor to construct and return the
    // rectangular ring.
    const std::vector<double> xCoords = {rect[0].first, rect[1].first,
                                         rect[2].first, rect[3].first,
                                         rect[0].first};
    const std::vector<double> yCoords = {rect[0].second, rect[1].second,
                                         rect[2].second, rect[3].second,
                                         rect[0].second};
    // Create a rectangular ring.
    Ring rectRing(ringID, shapeID, CLOSED_RING, 5,
                  &xCoords[0], &yCoords[0], infoList);
    rectRing.population = population;
    // Insanity checks
    ASSERT(rectRing.contains(node1.first, node1.second));
    ASSERT(rectRing.contains(node2.first, node2.second));
    // Return the newly created cring
    return rectRing;
}

/** A couple of convenience aliases to interface with boost::geometry
    library to handle intersection of rings. */
using boost_point = boost::geometry::model::d2::point_xy<double>;
using boost_ring  = boost::geometry::model::ring<boost_point>;

/** Internal helper method to convert to a boost ring.

    This helper method is placed here to reduce compile-time.
    Including boost headers in the Ring.h increases compile time by
    3x!

    \param[in] ring The ring to be converted to a boost_ring

    \param[in] check Optional flag to check to ensure that the ring is
    valid.
    
    \return Returns a boost_ring corresponding to the given ring.
*/
boost_ring
toBoostRing(const Ring& ring, const bool check = false) {
    // Alias namespace to keep namespaces short in this method.
    namespace bg = boost::geometry;  

    // Add each point in the given ring to the boost_ring.
    boost_ring br;  // The boost_ring being created    
    const int vertexCount = ring.getVertexCount();
    for (int i = 0; (i < vertexCount); i++) {
        const Point vert = ring.getVertex(i);
        boost::geometry::append(br, boost_point(vert.first, vert.second));
    }

    // If the user does not require checking then return the newly
    // minted boost_ring back to the caller.
    if (!check) {
        return br;  // checking was not required.
    }

    // Check to ensure that the ring is valid.
    bg::validity_failure_type failure;
    if (!bg::is_valid(br, failure)) {
        // Try to correct the failure.
        boost::geometry::correct(br);
        // And check again to see if the ring is now valid.
        if (!boost::geometry::is_valid(br, failure)) {
            std::cerr << "Invalid ring: #" << ring.getRingID()
                      << ", shapeID: #"    << ring.getShapeID()
                      << ", vertices: "    << vertexCount 
                      << ", boost error code: " << failure << std::endl;
        }
    }
    return br;  // The boost_ring version of the given ring.
}

// Determine if ring intersects.
bool
Ring::intersects(const Ring& ring) const {
    // Use boost::geometry to test for intersections. 
    const boost_ring this_ring  = toBoostRing(*this);
    const boost_ring other_ring = toBoostRing(ring);
    const bool check = boost::geometry::intersects(this_ring, other_ring);
    return check;  // true if rings intersect
}

Ring
Ring::intersection(const Ring& ring, const Ring::Kind kind, const int ringID,
                   const int shapeID, const double population,
                   const std::vector<Ring::Info> infoList,
                   const std::string& status, const bool subtraction) const {
    // Alias namespace to keep namespaces short in this method.
    namespace bg = boost::geometry;    

    // Use boost::geometry to compute the intersection polygon.
    const boost_ring this_ring  = toBoostRing(*this);
    const boost_ring other_ring = toBoostRing(ring);
    // The output of intersection in theory is a set of polygons. But
    // we expect only one.
    std::vector<boost_ring> resultPolys;
    bg::intersection(this_ring, other_ring, resultPolys);
    if (resultPolys.empty()) {
        return Ring();
    }
    // The intersection can sometimes result in more than 1 polygon
    // due to edge cases of overlapping lines.  In this situation, we
    // use the polygon with the largest area.
    size_t polyToUse = 0;  // Assume first one by default.
    if (resultPolys.size() > 1) {
        std::cout << "For ring# " << ring.getRingID() << ", shapeID: #"
                  << ring.getShapeID() << ", got " << resultPolys.size()
                  << " intersections.\n";
        // Find polygon with largest area.
        double maxArea = bg::area(resultPolys.at(polyToUse));
        // Check each sub-intersection polygon.
        for (size_t i = 0; (i < resultPolys.size()); i++) {
            const double polyArea = bg::area(resultPolys.at(i));
            std::cout << "\tSub-poly area: " << polyArea << std::endl;
            if (polyArea > maxArea) {
                // Found a larger polygon. Use that one.
                maxArea  = polyArea;
                polyToUse = i;
            }
        }
        std::cout << "Using intersection polygon #" << polyToUse << std::endl;
    }

    // Use only the first polygon or the polygon with largest area.
    boost_ring inter_poly = resultPolys.at(polyToUse);  

    // Now we need to convert the intersected boost_ring into our
    // conventional ring.  For this we need a vector of x & y coords
    // which we create below.
    std::vector<double> xCoords, yCoords;
    for (const boost_point& pt : inter_poly) {
        xCoords.push_back(pt.x());
        yCoords.push_back(pt.y());
    }

    // Now, create the intersection ring with all the necessary
    // attributes setup.
    Ring interRing(ringID, shapeID, kind, xCoords.size(), xCoords.data(),
                   yCoords.data(), infoList);
    // Setup a couple of other attributes
    interRing.setPopulation(population);
    interRing.setSubtractionFlag(subtraction);
    interRing.status = status;

    // Finally return the newly created intersection ring.
    return interRing;
}

void
Ring::correctWithBoost() {
    // Alias namespace to keep namespaces short in this method.
    namespace bg = boost::geometry;      
    // First convert to boost ring without correcting checks.
    const boost_ring br = toBoostRing(*this);
    // If this ring is already valid, then there is nothing further to
    // be done.
    if (bg::is_valid(br)) {
        return;  // already valid. Nothing further to be done.
    }
    // So the ring is currently not valid. Try to correct the issue
    const boost_ring br_corr = toBoostRing(*this, true);
    // If the correction successful, then update the verticies.
    if (!bg::equals(br, br_corr)) {
        // Looks like some corrections happened. Let's update our
        // verticies.
        int vertIdx = 0;
        for (const boost_point& pt : br_corr) {
            xCoords[vertIdx] = pt.x();
            yCoords[vertIdx] = pt.y();
            vertIdx++;
        }
    }
    // Log a message to let the user know this ring has been corrected
    std::cout << "Corrected ring #" << ringID << ", shape #" << shapeID
              << std::endl;
}

// This method first removes duplicate points in the ring (that make
// it concave instead of being convex) and then corrects rings with
// boost.
void
Ring::correctRing() {
    // Use an unordered map to disambiguate points
    std::set<Point> duplicates;
    // Keep adding points the map while checking for duplicates along
    // the way. We start from index 1 because the ring should have the
    // same 0th and last vertex.
    for (int i = 0; (i < vertexCount - 1); i++) {
        Point pt = getVertex(i);
        // Check if we have already encountered this point.
        if (duplicates.find(pt) != duplicates.end()) {
            // This point is a duplicate. This needs to be
            // fixed. Let's fix it by taking the average of
            // previous-and-next points.
            const int prevIdx = (vertexCount + i - 1) % vertexCount;
            ASSERT((prevIdx >= 0) && (prevIdx < vertexCount));
            const int nextIdx = (vertexCount + i + 1) % vertexCount;
            ASSERT((nextIdx >= 0) && (nextIdx < vertexCount));
            // Use the 3 points to compute averages
            const Point prevPt = getVertex(prevIdx);
            const Point nextPt = getVertex(nextIdx);
            // Compute the new x & y coordinates as averages
            double avgX = (prevPt.first  + nextPt.first ) / 2;
            double avgY = (prevPt.second + nextPt.second) / 2;
            // Check and increase the value if the change is pretty small.
            if ((std::abs(avgX - pt.first) < 1e-5) ||
                (std::abs(avgY - pt.second) < 1e-5)) {
                // Magnify the smaller of the 2 differences.
                const double diffX = avgX - pt.first, diffY = avgY - pt.second;
                if (std::abs(diffX) < std::abs(diffY)) {
                    avgX += (diffX * 100);  // Magnify the differences
                } else {
                    avgY += (diffY * 100);  // Magnify the differences
                }
            }
            // Update the coordinates for the point
            xCoords[i] = avgX;
            yCoords[i] = avgY;
            // Print the corrections for validation
            std::cout << "Ring #"  << ringID << ": changed point #" << i
                      << " ("      <<  pt.first << ", "   << pt.second
                      << ") to (" << avgX   << ", " << avgY << ")\n";
            // Update point for insertion below
            const Point oldPt = pt;
            pt = getVertex(i);
            ASSERT( pt != oldPt  );
            ASSERT( pt != prevPt );
            ASSERT( pt != nextPt );
        }
        // Add the point to the set of vertices.
        duplicates.insert(pt);
    }

    // Now correct order of vertices (if needed) with correctWithBoost
    correctWithBoost();
}

#endif
