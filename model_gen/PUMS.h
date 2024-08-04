#ifndef PUMS_H
#define PUMS_H

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

#include <string>
#include <unordered_map>
#include "ShapeFile.h"
#include "PUMSHousehold.h"
#include "Building.h"
#include "ArgParser.h"

/** \typedef HouseholdMap std::unordered_map<std::string, PUMSHousehold>;

    \brief An unordered map to quicky look-up household records given
    a household ID.

    The SERIALNO in the PUMS data that is used as the key to look-up
    values in this map.
*/
using HouseholdMap = std::unordered_map<std::string, PUMSHousehold>;

/**
 * Convenience class to encapsulate the Public Use Micro Sample (PUMS)
 * data.  PUMS data provides information about households and people
 * living in housholds in a given region.  The data can be downloaded
 * from https://www.census.gov/programs-surveys/acs/data/pums.html.
 * This class works with the following 4 files:
 *
 * <ol>
 *
 * <li><b>PUMS people data</b>: The data can be downloaded from
 * https://www.census.gov/programs-surveys/acs/data/pums.html.  These
 * files are of the format csv_<b>p</b>il.zip -- the 'p' in the file
 * path is the one that indicates this is a people micro sample.  This
 * data file is used to obtain information about age and occupation of
 * the people.</li>
 *
 *
 * <li><b>PUMS household data</b>: The data can be downloaded from
 * https://www.census.gov/programs-surveys/acs/data/pums.html.  These
 * files are of the format csv_<b>h</b>il.zip -- the 'h' in the file
 * path is the one that indicates this is a people micro sample.  This
 * data file is used to obtain information about the building size,
 * number of people in the building.</li>
 *
 * <li><b>PUMA Shape file</b>: This SHP file provides the polygons
 * associated with the PUMA area codes in the PUMS files.  The
 * polygons are used to determine overlapping regions for building
 * generation.</li>
 *
 * <li><b>PUMA database file</b>: This DBF file provides metadata
 * about the polygons in the SHP file.  The metadata is used to
 * determine the PUMA area code associated with the polygon in the
 * shape file.</li>
 *
 * </ol>
 *
 * These files are of the format csv_<b>p</b>il.zip -- the 'p' in the
 * file path is the one that indicates this is a people micro sample.
 * This path is used by the PUMS class to load the subset of data
 * necessary for building generation.
 */
class PUMS {
public:
    /** Constructor that does not do much. The data is loaded when the
        load method in this class is called from ModelGenerator::run
        method.
    */
    PUMS() {}

    /** The destructor that cleans-up any dynamic memory used by this
        class.
    */
    ~PUMS() {}

    /** Primary interface method to load the PUMA data from the given
        data files.  The optional minX, minY, maxX, and maxY values
        define the rectangular region of interest and only PUMA
        regions that lie within this part (fully or partially) are
        retained.  This is used to optimize the memory and processing
        costs as much as possible.

        \note This method must be called first before the loadPUMS
        method is called.

        \param[in] pumaShpPath Path to the PUMA shape file that
        contains the rings that describe the region of samples
        contained in the PUMS file.  The path is specified by the user
        via the \c --puma-shp command-line argument and is processed
        by ModelGenerator.

        \param[in] pumaDbfPath Path to the PUMA BDF file that contains
        the rings that describe the region of samples contained in the
        PUMS file.  The path is specified by the user via the \c
        --puma-dbf command-line argument and is processed by
        ModelGenerator.

        \param[in] minX An optional minimum longitude coordinate below
        with PUMA shapes can be ignored.

        \param[in] communities An optional shape file containing rings
        that provide finer boundaries on the regions to be included.

        \param[in] roundUpThreshold In the 2020 data set the PUMA
        areas along the edge of the lake in Chicago were made
        larger than the community lines.  Consequently, the
        overlap in PUMA is just about 79% instead of 100%.  Hence
        several households were not getting included.
        Consequently, this command-line option was added to allow
        rounding up of PUMA areas above the 80% threshold.  The
        default value is 1.0 which means no rounding is done.
        Setting this to 0.79 will round up all values above 79% to
        100%.
        
        \return This method returns zero on success. On errors it
        returns a non-zero error code.
    */
    int loadPUMA(const std::string& pumaShpPath, const std::string& pumaDbfPath,
                 const double minX = -180, const double minY = -90,
                 const double maxX = +180, const double maxY = +90,
                 const ShapeFile& communities = {},
                 double roundUpThreshold = 1.0);

    /** Top-level method to distribute households from PUMS data to
        the bulidings generated from OSM data.

        This method first loads the PUMS data for each household by
        calling the loadPUMSdata internal helper method.  The data is
        held in memory to make processing faster.
        
        Next, this method distributes population to all of the
        buildings in the puma shapes, one shape at a time by calling
        the internal distributePopulation method.

        \param[in,out] buildings The list of buildings from the OSM
        data to which the PUMS population is to be distributed.

        \param[in] pumsHouPath Path to the PUMS CSV file that contains
        housing data.  The path is specified by the user via the \c
        --pums-h command-line argument and is processed by
        ModelGenerator.

        \param[in] pumsPepPath Path to the PUMS CSV file that contains
        people's data.  The path is specified by the user via the \c
        --pums-p command-line argument and is processed by
        ModelGenerator.

        \param[in] pumsHouColNames The names of columns from the PUMS
        housing CSV file to be included in the housing and people data
        file.

        \param[in] pumsPepColNames The names of columns from the PUMS
        people CSV file to be included in the housing and people data
        file.        
    */
    void distributePopulation(std::vector<Building>& buildings,
                              const std::string& pumsHouPath,
                              const std::string& pumsPepPath,
                              const ArgParser::StringList& pumsHouColNames,
                              const ArgParser::StringList& pumsPepColNames) const;

    /** Obtain the ID of the PUMA shape ring at a given index.

        \param[in] index The index of the PUMA ring. This value is
        typically obtained via an earlier call to the findPUMAIndex
        method in this class.  It is assumed that this index is valid.

        \return The ring ID associated with the given PUMA index.
     */
    inline int getPUMAId(const int index) const {
        return puma.getRing(index).getRingID();
    }

    /** Returns a list of PUMA IDs for all PUMA regions currently
        being used.

        \return A string containing space separated list of PUMA Ids.
    */
    std::string getPUMAIds() const;
    
    /**
       Obtain the index of the PUMA ring to which a given coordinate
       belongs.  This method is typically used to determine the PUMA
       ID for a given building, given the centroid of the building.

       \note This method is thread-safe and can be safefly called from
       multiple threads.
       
       \param[in] vertex The longitude (vertex.first) and latitude
       (vertex.second) of the vertex (typically the centroid of a
       building) for which the PUMA index is to be returned by this
       method.

       \param[in] startPumaIndex An optional PUMA ring index from
       where the search is to be being.  This index provides an option
       to use geospatial locality to commence the search -- that is,
       the caller can store and pass-back the return value of this
       method.
       
       \return The PUMA index for the ring to which this building has
       been assigned.
    */
    int findPUMAIndex(const Point& vertex, const int startPumaIndex = 0) const;

    /** Finds the index of the PUMA ring for the given pumaID.

        This method searches the list of PUMA rings to find the ID of
        the puma ring for the given index.

        \param[in] pumaID The puma area ID to search for.

        \return The index of the puma ring corresponding to the given
        ID. If the given ID is not found, then this method returns -1.
     */
    int findPUMAIndex(const int pumaID) const;

    /** Helper method to dump PUMA areas along with a given set of
        rings.  This method is typically used for troubleshooting
        purposes.

        \param[in] extraRings The additional rings to be drawn along
        with the PUMA areas.

        \param[in] xfigPath Optional output xfig file to where the
        rings are to be drawn.
    */
    void drawPUMA(const ShapeFile& extraRings,
                  const std::string& xfigPath = "puma.fig",
                  const int scale = 1638400) const;

    /** Obtain the full list of PUMA rings.

        \return The full list of PUMA rings.
    */
    const std::vector<Ring> getPUMArings() const {
        return puma.getRings();
    }
    
    /** Returns the ring corresponding to the given PUMA ID.

        \param[in] index The index of the ring in the list of PUMA
        rings to be returned by this method.

        \return The ring corresponding to the given PUMA area.  If the
        specified index is not valid, then this method throws an exception.
    */
    const Ring& getPUMARing(const int index) const {
        return puma.getRing(index);
    }
    
protected:
    /** Internal helper method to load PUMS household data data from
        a given data file. 

        \note The loadPUMA method should be called first before this
        method is called.
        
        \param[in] pumsHouPath Path to the PUMS CSV file that contains
        housing data.  The path is specified by the user via the \c
        --pums-h command-line argument and is processed by
        ModelGenerator.

        \param[in] pumsColNames The names of columns from the PUMS CSV
        file to be included in the housing and people data file.

        \return This method returns an map containing the household
        information loaded from the PUMS CSV files.
    */
    HouseholdMap loadHousehold(const std::string& pumsHouPath,
                               const ArgParser::StringList& pumsColNames) const;

    /** Internal helper method to load PUMS people data data from
        a given data file into correspding households.

        \note The loadHousehold method should be called first before
        this method is called.

        \param[in,out] households The set of households for which
        people data is to be populated by this method.
        
        \param[in] pumsPepPath Path to the PUMS CSV file that contains
        people's data.  The path is specified by the user via the \c
        --pums-p command-line argument and is processed by
        ModelGenerator.

        \param[in] pumsColNames The names of columns from the PUMS CSV
        file to be included in the housing and people data file.
    */    
    void loadPeopleInfo(HouseholdMap& households,
                        const std::string& pumsPepPath,
                        const ArgParser::StringList& pumsColNames) const;

    /** Internal helper method to convert column names to index
        positions based on the information on the CSV title lines.

        \param[in] titles The 1st title row in the CSV file from where
        the column names are to be loaded.

        \param[in] pumsColNames The list of desired column names.
    */
    std::vector<int>
    toColIndex(const std::string& titles,
               const ArgParser::StringList& pumsColNames) const;

    /** Helper method to combine information in specific columns into
        a CSV string.

        \param[in] colIndexs The index of the columns whose values are
        to be combined into a CSV

        \param[in] info The information to be used to build the CSV

        \return Returns a string in CSV format with the information
        for specific column indexs.
    */
    std::string toCSV(const std::vector<int>& colIndexs,
                           const std::vector<std::string>& info) const;

    /** Helper method to distribute households from PUMS data to
        the bulidings generated in a specific PUMA area.

        \note This method is called from multiple threads.

        \param[in] pumaID The ID of the PUMA area for which the PUMS
        data is to be distributed.

        \param[in] popFrac The fraction of the households to be
        assigned.  This fraction is for parts that overlap. If a shape
        fully overlaps, then this value should be 1.0.
        
        \param[in,out] buildings The list of buildings from the OSM
        data to which the PUMS population is to be distributed.

        \param[in,out] households The households whose population is
        to be distributed to various buildings.
    */
    void distributePopulation(int pumaID, const double popFrac,
                              std::vector<Building>& buildings,
                              HouseholdMap& households) const;

    /** Internal helper method to compute finer overlapping areas
        between a given PUMA ring and community shapes.

        \param[in] pumaRng The PUMA ring with which finer overlaps is
        to be computed.

        \param[in] communities The shape file containing the list of
        community shapes being used.  If the shape file does not
        contain any rings then this method immediately returns the
        defOverlap value.

        \param[in] defOverlap A default overlap value to be returned
        by this method if the communities shape file does not contain
        any rings.
    */
    double getIntersectionOverlap(const Ring& pumaRng,
                                  const ShapeFile& communities,
                                  const double defOverlap) const;

    /** \typedef BldIdxSqFt std::pair<int, int>

        This pair as index of building and the square footage of the
        building.  This is used to sort the buildings based on their size.
    */
    using BldIdxSqFt = std::pair<int, int>;
    
    /** \typedef HouIdSize std::pair<std::string, size_t>

     This pair is used to hold PUMS household record for the given
     PUMA region sorted on BLD and then on number of bedrooms.  Here we
     use a pair with housing ID and (BLD * 100 + BDSP) for sorting.
    */
    using HouIdSize = std::pair<std::string, size_t>;

    /** Get a list of buildings for a given PUMA area sorted based on
        the square footage of the building.

        \param[in] pumaID The ID of the PUMA regions whose buildings
        are to be used.

        \param[in] buildings The list of buildings from where the
        information is to be obtained and returned.

        \return A vector containing a pair of values with \c first
        having index of the building in the buildings list and \second
        having square footage.
    */
    std::vector<BldIdxSqFt>
    getSortedBldList(const int pumaID,
                     const std::vector<Building>& buildings) const;

    /** Get a list of households for a given PUMA area sorted based on
        the building code and number of bedrooms.

        \param[in] pumaID The ID of the PUMA regions whose households
        are to be used.

        \param[in] households The list of all households from where
        the subset of households for the given PUMA region are to be
        extracted and sorted.
        returned.

        \return A vector containing a pair of values with \c first
        having household IDt and \second having household size used
        for sorting.
    */
    std::vector<HouIdSize>
    getSortedHouList(const int pumaID, const HouseholdMap& households) const;

private:
    /** Internal helper method to assign single family households to
        the smaller buildings.
        
        \param[in,out] buildings The list of buildings from the OSM
        data to which the PUMS population is to be distributed.

        \param[in,out] households The households whose population is
        to be distributed to various buildings.

        \param[in] bldSzList The list of buildings in a given PUMA
        region sorted based on the square footage of the
        building. Families are assigned in the order of the building
        sizes.

        \param[in] houSzList The list of households sorted based on
        the building size estimate for the household.

        \param[in] popFrac The fraction of the households to be
        assigned.  This fraction is for parts that overlap. If a shape
        fully overlaps, then this value should be 1.0.
        
        \param[in,out] hldIdx The starting index in the houSzList from
        where the households are to be assigned.  This value is
        updated to reflect the households that have been assigned.

        \param[in,out] bldIdx the starting building index from where
        the households are to be assigned.  This value is updated to
        reflect the people who have been assigned.

        \param[in] pumaID The PUMA area ID for including in error
        messages.
    */
    int assignSingleFamilies(std::vector<Building>& buildings,
                              HouseholdMap& households,
                              const std::vector<BldIdxSqFt> bldSzList,
                              const std::vector<HouIdSize> houSzList,
                              const double popFrac,
                              size_t& hldIdx, size_t& bldIdx,
                              const int pumaID) const;
    
private:
    /** The set of PUMA rings that are being used to generate
        population for the different buildings in the given area.  The
        rings in this list are populated via call to the load method
        in this class.
    */
    ShapeFile puma;
};

#endif  // PUMS_H
