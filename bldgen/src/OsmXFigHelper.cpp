#ifndef OSM_XFIG_HELPER_CPP
#define OSM_XFIG_HELPER_CPP

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
#include <iostream>
#include <sstream>
#include <cmath>
#include <fstream>
#include "OsmXFigHelper.h"

// Convenience shortcuts to access boost namespace
using namespace boost::asio;
using namespace boost::asio::ip;

OsmXFigHelper::OsmXFigHelper(const std::string& cacheDir,
                             const std::string& url) :
    cacheDir(cacheDir), baseURL(url) {
    // Nothing else to be done here for now.
}

OsmXFigHelper::~OsmXFigHelper() {
    // Nothing else to be done here for now.
}

Coord
OsmXFigHelper::getXYValues(double latitude, double longitude, const int size,
                           const bool wrap) const {
    constexpr double PI = 3.1415926;
    // First normalize the latitude and a longitude and normalises the
    // values so they can be scaled to any size
    if ((wrap) && (longitude > 180)) {
        longitude -= 360;
    }
    longitude /= 360;
    longitude += 0.5;
    if (latitude <= -90) {
        latitude = 1;
    } else if (latitude >= 90) {
        latitude = 0;
    } else {
        latitude *= (M_PI / 180.0); // Conver to radians
        latitude = 0.5 - ((log(tan((PI / 4) + (0.5 * latitude))) / PI) / 2.0);
    }
    // Now calculate the pixel value using normalized latitude and
    // longitude.
    const int x = (int) (longitude * size);
    const int y = (int) (latitude  * size);
    // Return the pixel coordinates back
    return Coord(x, y);
}

Location
OsmXFigHelper::getLatLon(const int x, const int y, const int MapSize) const {
    double lat = 0, lon = 0;
    lat = (double) ((MapSize / 2) - y) / (MapSize / (2 * M_PI));
    lon = (double) x / MapSize;
    // Put X back to the way it was (longitude)
    lon -= 0.5;
    lon *= 360;
    // Y is going to be trickier! (latitude)
    lat = Utils::toDegrees(atanl(sinhl(lat)));
    // Return the converted geolocations
    return Location(lat, lon);
}

Location
OsmXFigHelper::getTileInfo(const int x, const int y, const int zoom) const {
    const int size = ImgTiles(zoom);  // This is 2^zoom.
    // Returns the top-left latitude and longitude values.
    return getLatLon(x, y, size);
}

void
OsmXFigHelper::setMinXY(const double latitude, const double longitude,
                        const int zoom) {
    minXY = getXYValues(latitude, longitude, ImgSize(zoom));
}

std::string
OsmXFigHelper::getURL(const double lat, const double lon,
                      const int zoom) const {
    const int size = ImgTiles(zoom);   // Shortcut for 2^zoom
    // Use helper method to convert lat, lon to image-tile's x, y
    const Location xy = getXYValues(lat, lon, size);
    // Construct URL to image on open street maps
    return getURL(xy.first, xy.second, zoom);
}

std::string
OsmXFigHelper::getURL(const int x, const int y, const int zoom) const {
    std::ostringstream url;
    url << baseURL << "/" << zoom << "/" << x << "/" << y << ".png";
    return url.str();
}

std::string
OsmXFigHelper::getFileName(const double lat, const double lon,
                           const int zoom) const {
    const int size = ImgTiles(zoom);   // Shortcut for 2^zoom
    // Use helper method to convert lat, lon to image-tile's x, y
    const Coord xy = getXYValues(lat, lon, size);
    // Construct path to image tile in cache directory
    return getFileName(xy.first, xy.second, zoom);
}

std::string
OsmXFigHelper::getFileName(const int x, const int y, const int zoom) const {
    // Construct path to image tile in cache directory
    std::ostringstream path;
    path << cacheDir << "/zoom_" << zoom
         << "_x_" << x << "_y_" << y << ".png";
    return path.str();
}

bool
OsmXFigHelper::download(const std::string& url,
                        const std::string& path) const {
    // Extract server from the URL
    const size_t pos = url.find('/', 7);
    std::string server = url.substr(7, pos - 7);
    // Create a client socket to download data from server
    tcp::iostream socket(server, "80");
    // Send HTTP get request to the server
    socket << "GET "   << url.substr(pos) << " HTTP/1.1\r\n"
           << "Host: " << server << "\r\n"
           << "Connection: Close\r\n\r\n";
    // Read response from server to ensure it is 200 OK.
    std::string line;
    std::getline(socket, line);
    if (line.find("200 OK") == std::string::npos) {
        std::cerr << "Error resource " << url << " not found.\n";
        return false;
    }
    // Skip HTTP readers from the server to get to image data
    while (line != "\r" && std::getline(socket, line)) {}
    // Write the image data to the specified file
    std::ofstream img(path);
    if (!img.good()) {
        std::cerr << "Unable to write to file: " << path << std::endl;
        return false;
    }
    // Copy binary image data until EOF
    char buffer[4096];
    while (socket.read(buffer, 4096), socket.gcount() > 0) {
        img.write(buffer, socket.gcount());
    }
    return true;
}

bool
OsmXFigHelper::checkDownload(const std::string& url,
                             const std::string& path) const {
    std::ifstream img(path);
    if (img.good()) {
        return true;  // File already exists.
    }
    // Download the file as it does not exist
    // Print information to the user for long running downloads
    std::cout << "Downloading image for: " << path << "...             \r";
    return download(url, path);
}

#endif
