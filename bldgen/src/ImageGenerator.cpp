#ifndef IMAGE_GENERATOR_CPP
#define IMAGE_GENERATOR_CPP

//------------------------------------------------------------
//
// This file is part of HAPLOS <http://pc2lab.cec.miamiOH.edu/>
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

#include "Boost.h"
#include "ImageGenerator.h"
#include "OsmXFigHelper.h"

ImageGenerator::ImageGenerator(int zoomLevel, const std::string& cacheDir) :
    figHelp(cacheDir), zoom(zoomLevel) {
    // Nothing else to be done for now.
}

ImageGenerator::~ImageGenerator() {
    // Nothing else to be done for now.
}

void
ImageGenerator::setTopLeftCoord(const OsmSection& osm) {
    // The bounds of the OSM region for which an image is being generated.
    double tlLat = 0, tlLon = 0, brLat = 0, brLon = 0;    
    // First get the top-left and bottom-right lat, lon
    osm.getBounds(brLat, tlLon, tlLat, brLon);
    // Convert it to tile coordinates first.
    const int NumTiles = OsmXFigHelper::ImgTiles(zoom);
    const Coord minXY = figHelp.getXYValues(tlLat, tlLon, NumTiles);
    // Now determine the latitude and longitude for the top-left of tile.
    const Location minLatLon = figHelp.getLatLon(minXY.first, minXY.second,
                                                 NumTiles);
    // Now initialize map tile <x, y> coordinates based on bounds.
    // This final step uses pixel coordinates (instead of tile coordinates).
    figHelp.setMinXY(minLatLon.first, minLatLon.second, zoom);
    // Finally draw a rectangle around the actual bounds for this
    // section so we clearly know the exact area we are working on
    const int size = OsmXFigHelper::ImgSize(zoom);  // full size of map image
    const Coord tl = figHelp.toFig(figHelp.getXYValues(tlLat, tlLon, size));
    const Coord br = figHelp.toFig(figHelp.getXYValues(brLat, brLon, size));
    xfig.drawRect(tl.first, tl.second, br.first - tl.first,
                  br.second - tl.second, RED, BLACK, NO_FILL, 58, 2);
}

void
ImageGenerator::generate(const BuildingGenerator& bg,
                         const std::string& figFilePath,
                         const bool drawWays, const bool drawBuildings) {
    // Setup the xfig helper and start fig file.
    xfig.setOutput(figFilePath, 1200, 2);
    // Setup the top-left image x, y values to translate latitudes and
    // longitudes relative to it.
    setTopLeftCoord(bg.getSection());
    // Draw all the map tile images for the background.
    const OsmSection& osm = bg.getSection();
    generateMapTiles(osm);
    // Draw all the ways in the section (if asked for)
    if (drawWays) {
        for (const OsmWay& way : osm.getWays()) {
            draw(way, osm);  // helper method to draw 1 way
        }
    }
    // Draw all the buildings in the section (if asked for)
    if (drawBuildings) {
        for (const OsmBuilding& bldg : bg.getBuildings()) {
            draw(bldg);  // helper method to draw 1 building
        }
    }
}

void
ImageGenerator::draw(const OsmBuilding& bldg) {
    const int size = OsmXFigHelper::ImgSize(zoom);
    // Convert geolocations to relative image coordinates
    const Location loc = bldg.getLocation();
    const Coord center = figHelp.toFig(figHelp.getXYValues(loc, size));
    // Convert radius in miles to change in latitude, longitude to
    // find x-radius and y-radius to draw building.
    const Location top   = Utils::change(loc, bldg.getRadius(), 0);
    const Location right = Utils::change(loc, 0, bldg.getRadius());
    // Convert difference in longitude to fig x-radius
    const Coord ritCoord = figHelp.toFig(figHelp.getXYValues(right, size));
    const int   xRad     = std::max(1, std::abs(center.first - ritCoord.first));
    // Convert difference in latitude to fig y-radius
    const Coord topCoord = figHelp.toFig(figHelp.getXYValues(top, size));
    const int   yRad     = std::max(1, std::abs(center.second -
                                                topCoord.second));
    // Draw an oval representing the area covered by building
    xfig.drawOval(center.first, center.second, xRad, yRad, BLACK, CYAN);
}

void
ImageGenerator::draw(const OsmWay& way, const OsmSection& osm) {
    const int size = OsmXFigHelper::ImgSize(zoom);  // full size of map image
    const NodeList& nodeIDs = way.getNodeIDs();
    // Set up information about the way as a comment
    std::ostringstream oss;
    oss << "way [id: " << way.getID() << "], "
        << dynamic_cast<const OsmTags&>(way);
    xfig.addComment(oss.str());
    // Start a series of points connected by lines to draw way
    xfig.startPolyLine(nodeIDs.size(), BLACK, 54);
    for (size_t i = 0; (i < nodeIDs.size()); i++) {
        // Get the node objects for the 2 consecutive node IDs
        const Location curr = osm.getNode(nodeIDs[i]).getLocation();
        // Convert their geolocations to relative image coordinates
        const Coord node    = figHelp.toFig(figHelp.getXYValues(curr, size));
        // Now add line to the figure being generated
        xfig.addVertex(node.first, node.second);
    }
    // Finish the polyline
    xfig.endPolyLine();
}

// Genreates a grid of images to cover the region indicated by the
// OsmSection object supplied to this method.
void
ImageGenerator::generateMapTiles(const OsmSection& osm) {
    // Note: Here the map size is computed differently (in terms of
    // tiles and not pixels) to ease working with image tiles.
    const int size = OsmXFigHelper::ImgTiles(zoom);   // Number of tiles
    // The bounds of the OSM region for which an image is being generated.
    double tlLat = 0, tlLon = 0, brLat = 0, brLon = 0;
    // First get the top-left and bottom-right lat, lon
    osm.getBounds(brLat, tlLon, tlLat, brLon);
    // Now initialize map tile <x, y> coordinates based on bounds
    // int minX = 0, minY = 0, maxX = 0, maxY = 0;
    const Coord minXY = figHelp.getXYValues(tlLat, tlLon, size);
    const Coord maxXY = figHelp.getXYValues(brLat, brLon, size);
    // Now iterate over the grid and create map tile entries.  Here we
    // assume each map tile is exactly 256x256 pxiel image.
    constexpr int scale = 256 * PIXEL_TO_XFIG_SCALE;
    for (int x = minXY.first; (x <= maxXY.first); x++) {
        for (int y = minXY.second; (y <= maxXY.second); y++) {
            std::string img = figHelp.getFileName(x, y, zoom);
            // Download the image file into cache directory (if needed)
            figHelp.checkDownload(figHelp.getURL(x, y, zoom), img);
            // Add map tile to the xfig file.
            xfig.addImage(img, (x - minXY.first) * scale,
                          (y - minXY.second) * scale, scale, scale);
        }
    }
}


#endif
