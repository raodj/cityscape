#ifndef OSM_XFIG_HELPER_H
#define OSM_XFIG_HELPER_H

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

#include <string>
#include "Utils.h"
#include "XFigHelper.h"

/** Alias to work with <x, y> coordinates from a figure perspective.
    Coord.first has x-coordinate while Coord.second has y-coordinate.
    This information is used to translate <latitude, longitude> pairs
    to image/figure coordinates.
*/
using Coord = std::pair<int, int>;

/** Convenience class to help managing OSM map tiles.

    The class is a helper class that is used to draw maps and place
    building generated to verification of the building generation
    process.  Specifically this class provides the following features:

    <ul>

    <li>A cache directory where the image tiles are stored.  The cache
    is used so that they don't need to be re-downloaded each time an
    map is created.</li>

    <li>Given a latitude, longitude, and zoom-level this class
    provides the URL from where the file can be downloaded.</li>
    
    </ul>

    \see ImgSize, ImgTiles methods
*/
class OsmXFigHelper {
public:
    /** The constructor (acts as default) with optional parameters.

        The constructor merely initializes the instance variables to
        default initial values.

        \param[in] cacheDir The directory where the image tiles are
        downloaded and cached for use with map generation.
        
        \param[in] url The URL to the open street maps from where the
        tile images are to be downloaded.
    */
    OsmXFigHelper(const std::string& cacheDir = "../cache",
                  const std::string& url = "http://tile.openstreetmap.org");

    /** The destructor.

        This is just a place holder as the destructor does not have
        any special tasks to perform (because this class does not
        directly use any dynamic memory).
     */
    ~OsmXFigHelper();

    /** Convert latitude and longitude to X and Y pixel coordinates.

        This is a convenience method that is used to generate the <x,
        y> pixel coordinate values from a given <latitude, longitude>
        values at a given image size.

        \param[in] latitude The latitude (-90 < latitude < 90) to be
        converted to Y coordinate.

        \param[in] longitude The longitude (-180 < longitude < 180) to
        be converted to X coordinate.

        \param[in] size The full size of the image at a given zoom
        level.  See ImgSize, ImgTiles methods in this class.

        \param[in] wrap If the flag is true, this method wrap's values
        of latitude and longitude if they exceed stipulated ranges.

        \return Returns a Coord pair that contains <x, y> pixel values.

        \note This method does not perform any adjustments with
        respect to the the minXY coordinates.
    */
    Coord getXYValues(double latitude, double longitude, const int size,
                      const bool wrap = true) const;

    /** Convert latitude and longitude to X and Y pixel coordinates.

        This is a convenience method that is used to generate the <x,
        y> pixel coordinate values from a given <latitude, longitude>
        values at a given image size.

        \param[in] loc The location pair containing the latitude (in
        loc.first) and longitude (in loc.second).

        \param[in] size The full size of the image at a given zoom
        level.  See ImgTiles or ImgSize methods.

        \param[in] wrap If the flag is true, this method wrap's values
        of latitude and longitude if they exceed stipulated ranges.

        \return Returns a Coord pair that contains <x, y> pixel values.

        \note This method does not perform any adjustments with
        respect to the the minXY coordinates.
    */
    Coord getXYValues(const Location& loc, const int size,
                      const bool wrap = true) const {
        return getXYValues(loc.first, loc.second, size, wrap);
    }
    
    /** Convert a given <x, y> pixel value to <latitude, longitude>
        values.

        This is a convenience method that is used to generate the
        <latitude, longitude> from a given <x, y> pixel coordinate
        value in a image of given size.

        \param[in] x The pixel's x coordinate.

        \param[in] y The pixel's y coordinate.

        \param[in] size The full size (either width or height in
        pixels) of the image at a given zoom level.  See ImgTiles and
        ImgSize methods in this class.

        \return This method returns a Location object containing the
        <latitude, longitude> value for the given <x, y> pixel values.

        \note This method does not perform any adjustments with
        respect to the the minXY coordinates.
    */
    Location getLatLon(const int x, const int y, const int size) const;

    /** Obtain the URL from where the tile image for the given
        latitude and longitude can be downloaded.

        \param[in] lat The latitude which must be visible in the image
        tile at the given zoom level.

        \param[in] lon The longitude which must be visible in the
        image tile at the given zoom level.

        \param[in] zoom The zoom level at which the map tile is to be
        visisble.

        \return The URL from where the Tile image is to be downloaded.
    */
    std::string getURL(const double lat, const double lon,
                       const int zoom) const;

    /** Obtain the URL from where the tile image for the given
        latitude and longitude can be downloaded.

        \param[in] x The x-coordinate of the tile in the given
        zoom-level.

        \param[in] y The y-coordinate of the tile in the given zoom
        level.

        \param[in] zoom The zoom level at which the x and y
        coordinates are  specified.

        \return The URL from where the Tile image is to be downloaded.
    */
    std::string getURL(const int x, const int y, const int zoom) const;

    /** Obtain the path to the cache image file from where the image
        tile can be obtained.

        \param[in] lat The latitude which must be visible in the image
        tile at the given zoom level.

        \param[in] lon The longitude which must be visible in the
        image tile at the given zoom level.

        \param[in] zoom The zoom level at which the map tile is to be
        visisble.

        \return The path to the cache time image file.
    */
    std::string getFileName(const double lat, const double lon,
                            const int zoom) const;

    /** Obtain the path to the cache image file from where the image
        tile can be obtained.

        \param[in] x The x-coordinate of the tile for which image
        name is to be returned.

        \param[in] y The y-coordinate of the tile for which the image
        name is to be returned.

        \param[in] zoom The zoom level at which the map tile is to be
        visisble.

        \return The path to the cache time image file.
    */
    std::string getFileName(const int x, const int y, const int zoom) const;
    
    /** Obtain the top-left coordinate of a OSM map image tile.

        This method is a convenience method that can be used to obtain
        the top-left coordinate of a OSM map image tile.  The tile is
        specified by the triplet <x, y, zoom>, where <x, y> are tile
        coordinates at the given zoom level.

        \param[in] x The x-coordinate of the Map tile.

        \param[in] y The y-coordinate for the Map tile.

        \param[in] zoom The zoom level associated with the <x, y> tile
        coordinates.

        \return The top-left latitude and longitude values.
    */
    Location getTileInfo(const int x, const int y, const int zoom) const;

    /** Convenience method to download a map tile from the given URL
        to a given file.

        \param[in] url The URL from where the image is to be
        downloaded.

        \param[in] path The path to the image (typically in the cache
        directory) to where the image file is to be saved.

        \return This method returns true if the file was successfully
        downloaded.
        
        \note This method provides a very simple HTTP client. Possibly
        it should be rewritten to use Curl libraries instead of boost.
    */
    bool download(const std::string& url, const std::string& path) const;

    /** This is a convenience method to check if the image tile exists
        and if not download it.

        This is a convenience method -- it first checks to see if file
        exists at the given path.  If not, the file is downloaded from
        the specified URL.

        \param[in] url The URL from where the image is to be
        downloaded.

        \param[in] path The path to the image (typically in the cache
        directory) to where the image file is to be saved.

        \return This method returns true if the file was successfully
        downloaded.        
    */
    bool checkDownload(const std::string& url, const std::string& path) const;

    /** Set the logical top-left corner of the image being generated
        to ease translation of coordinates.

        The minimum <x, y> values for the top-left corner of the
        images being generated.  This value is used to adjust the
        absolute pixel value for a given <latitude, longitude> pair
        to relative pixel positions within the image being generated.
        This value is set in the setMinXY method and used in the
        toFig method.

        \param[in] latitude The latitude (-90 < latitude < 90) to be
        used as the minimum Y coordinate.

        \param[in] longitude The longitude (-180 < longitude < 180) to
        be used as the minimum X coordinate.

        \param[in] zoom The zoom level of the image.  This is used to
        compute the full map size via call to ImgSize method.
    */
    void setMinXY(const double latitude, const double longitude,
                  const int zoom);

    /** Convenience method to translate an absolute <x,y> coordinate
        to relative values.

        This method is a convenience method that can be used to
        translate a given absolute (w.r.t the whole map image which
        can be million pixels wide & tall) to relative pixel values
        which can be just 100s of pixels wide/tall).  The conversion
        is done using the minXY value set via the setMinXY method.

        \param[in] xy The <x, y> coordinate pairs to be translated.

        \param[in] scale An optional scale to translate pixel position
        into XFig coordinates.
        
        \return Coord The translated coordinate pair.
     */
    inline Coord toFig(const Coord& xy,
                       const int scale = PIXEL_TO_XFIG_SCALE) const {
        return toFig(xy.first, xy.second, scale);
    }

    /** Convenience method to translate an absolute <x,y> coordinate
        to relative values.

        This method is a convenience method that can be used to
        translate a given absolute (w.r.t the whole map image which
        can be million pixels wide & tall) to relative pixel values
        which can be just 100s of pixels wide/tall).  The conversion
        is done using the minXY value set via the setMinXY method.

        \param[in] x The x image/pixel coordinate to be translated.

        \param[in] y The y image/pixel coordinate to be translated.        

        \param[in] scale An optional scale to translate pixel position
        into XFig coordinates.
        
        \return Coord The translated coordinate pair.
     */
    inline Coord toFig(int x, int y, int scale = PIXEL_TO_XFIG_SCALE) const {
        return Coord((x - minXY.first)  * scale,
                     (y - minXY.second) * scale);
    }

    /** Convenience method to determine size of the full map image in
        number of pixels.

        In OSM, maps are square images with width pixels representing
        -180 to +180 longitudes and height pixels representing +90 to
        -90 latitude.  The width and height of a map is dependent on
        the zoom level and is defined by the formula: width or height
        = 2<sup>zoom</sup> * 256, where 256 is the size of each tile
        in the map.

        \param[in] zoom The zoom level at which the map/image is being
        generated.
        
        \return The number of pixels in the image.
    */
    static int ImgSize(int zoom) { return (1 << zoom) << 8; }

    /** Convenience method to determine number of tiles that
        constitute a map image.

        In OSM, maps are square images that are broken into a number
        of 256x256 tile images.  The number of tiles (along one axis,
        either x or y) that constitute an image is zoom level and is
        defined by the formula: 2<sup>zoom</sup>.

        \param[in] zoom The zoom level at which the map/image is being
        generated.

        \return The number of tiles along any one axis of the map
        image.
    */
    static int ImgTiles(int zoom) { return (1 << zoom); }

private:
    /** The directory where the OSM tile images are stored.  This
        directory is typically set to a relative path.  This value is
        set in the constructor and is never changed during the life
        time of this object.
    */
    std::string cacheDir;

    /** The base URL from where the tile images are to be obtained.
        This is by default set to: http://tile.openstreetmap.org.
        This value si set in the constructor and is never changed
        during the life time of this object.
     */
    std::string baseURL;

    /**  The minimum <x, y> values for the top-left corner of the
         images being generated.  This value is used to adjust the
         absolute pixel value for a given <latitude, longitude> pair
         to relative pixel positions within the image being generated.
         This value is set in the setMinXY method and used in the
         toFig method.

     */
    Coord minXY;
};

#endif
