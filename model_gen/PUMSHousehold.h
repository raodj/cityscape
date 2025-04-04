#ifndef PUMS_HOUSEHOLD_H
#define PUMS_HOUSEHOLD_H

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
#include <vector>
#include <unordered_map>
#include "PUMSPerson.h"

/** Shortcut to refer to PWGTP and person info for each household */
using PepWtInfo = std::pair<int, PUMSPerson>;

/** A list of occurrence and user information values for each household */
using PepWtInfoList = std::vector<PepWtInfo>;

/** An alias for a map to quickly look-up person entries via their
    ID. This map is only used when an already-generated model is being
    read. See PUMSHousehold::read and OSMData::loadModel
*/
using PersonMap = std::unordered_map<int, PUMSPerson>;

/** Class to encapsulate necessary information about a household and
    people living in them.

    This class is used to encapsulate information about each household
    that is used to pulate the buildings in the generated model.  Each
    household is identified by a unique SERIALNO set for them in the
    PUMS files.

    For column information see: https://www2.census.gov/programs-surveys/acs/tech_docs/pums/data_dict/PUMS_Data_Dictionary_2017-2021.pdf
*/
class PUMSHousehold {
public:
    /** A default constructor to ease adding households to standard
        containers.  The constructor merely initializes the values
        to invalid settings.
    */
    PUMSHousehold() {}

    /** Convenience constructor to create a huousehold record with the
        necessary information pre-set.

        \param[in] houseInfo The information about this household to
        be retained.  This string constains a set of CSV.

        \param[in] bedRooms The number of bedrooms in this household.

        \parma[in] bldCode The PUMS building code for this building in
        the range 1 through 9 (inclusive).

        \param[in] pumaID The PUMA area code for this household.

        \param[in] wgtp The weight for this PUMA record that
        indicates the number of occurrences of this type of household.

        \param[in] hincp The annual household income for this record.
    */
    PUMSHousehold(const std::string houseInfo, int bedRooms, int bldCode,
                  int pumaID, int wgtp, int hincp, int people);

    /** Convenience constructor to create a huousehold record with the
        necessary information pre-set.

        \param[in] hld The basic household information.

        \param[in] bldId The actual building ID to which this
        household has been assigned.

        \param[in] people The numnber of people to be set for this
        household.

        \param[in] peopleInfo The information about the people.
    */
    PUMSHousehold(const PUMSHousehold& hld, int bldId, int people,
                  const std::vector<PUMSPerson>& peopleInfo);
    
    /** The destructor.

        Has nothing much to do because this class currently does not
        use any dynamic memory.
     */
    ~PUMSHousehold() {}

    /** Helper method to add a person to this household.

        \param[in] occurrence The number of occurrences of this
        specific person record.
        
        \param[in] person The information about the person to be
        added.  This information is encapsulated in the object. The
        columns are in the order specified by the user.
    */
    void addPersonInfo(const int occurrence, const PUMSPerson& person);

    /** Get a size estimate for this household based on apartment
        configuration and number of bedrooms.  This method returns a
        value in the range 0 to 1099 based on the apartment code and
        number of bedrooms.  The return value is useful for sorting
        the households based on the estimated building size to which
        they are to be assigned.

        \return An estimated size of the building that this household
        lives in.
    */
    size_t getSize() const {
        // return BldRecode[bld] * 100 + bedRooms;
        return bld * 100 + bedRooms;
    }

    /**
       This method can be used to obtain the average number of
       apartments in a given building.  This is just a convenience
       method that can be used.  The return values for this instance
       method are:

        BLD Value |  AptsInBuilding |  Description
        ----------|-----------------|--------------
           bb     |        99       | Group quarters
           01     |         1       | Mobile home / trailer
           02     |         1       | One family home attached
           03     |         1       | One family home detached
           04     |         2       | 2 apartments
           05     |         4       | 3-4 apartments in building
           06     |         7       | 5-9 apartments in building
           07     |         15      | 10-19 apartments in building
           08     |         35      | 20-49 apartments in building
           09     |         50      | > 50 apartments in building


        \return An estimate of number of apartments that may be
        present in a buliding in which this household lives.

        \see getBld
    */       
    int getAptInBld() const;

    /** Obtain the number of families or households of this type.

        \return The number of familities of households of this
        type. This value is the same as the WGTP value in the CSV.
    */
    int getCount() const { return wgtp; }

    /** Obtain the number of people in this household.

        \return The nunber of people. The return value is -1 if this
        household is a template.
     */
    int getPeopleCount() const  { return people; }

    /** Obtain information about the people in this household.

        \return The information about the people in the household.
     */
    const std::vector<PUMSPerson>& getPeopleInfo() const { return peopleInfo; }
    
    /** Generate information about the i'th household of this type.

        This method is a convenience method that can be used to obtain
        information about the i'th household of this type.  Household
        information is a string containing information about the
        people in the household. People information is a set of CSV
        values as specified by the user. Each person information is
        separated by semicolons.

        \parma[in] i The household for which information is to be
        returned.  0 <= i < getCount()

        \param[out] pepCount The number of people in the household
        information returned by this method.

        \return A list of people living in the household
        information. Note in fractional cases, the returned list can
        be an empty string and such returns should be ignored.
     */
    std::vector<PUMSPerson> getInfo(const int i, int& pepCount) const;

    /** Obtain the type of building set for this household.

        \return The BLD code as set in the PUMS data for this
        household.

        \see getAptInBld
    */
    int getBld() const { return bld; }

    /** Get the number of bedrooms that this household lives in.

        \return The number of bedrooms for this household. This value
        is essentially the same as the BDSP column in the PUMS data,
        but the minimum value is set to 1.
    */
    int getRooms() const { return std::max(1, bedRooms); }

    /** Obtain the PUMA ID associated with this household.

        \return The PUMA ID associated with this household.
    */
    int getPUMAId() const { return pumaID; }

    /** Obtain the building to which this household has been assigned.

        \return The building ID. It is -1 if this household is not
        assigned to an building.
     */
    int getBuildingID() const { return buildingID; }

    /** Obtain family income set for this household.

        \return The family income (which is the exactly the same as
        the hincp PUMA attribute).
     */
    int getFamilyIncome() const { return hincp; }
    
    /** Load the data for this household from a given input stream.

        This is a helper method that is used to read information for
        this household from a given input stream.  This method assumes that
        the data has been written by a prior call to the write method
        in this class.

        \param[in,out] is The input stream with 1 line of data from
        where the information for this household is to be read.

        \param[in] personMap The map to be used to look-up a person's
        information associated with this household.
    */
    void read(std::istream& is, const PersonMap& personMap);

    /** Write this household information to a given output stream.

        This is a helper method that is used to write the information
        associated with this household to a given output stream.

        \param[out] os The output stream to where the information
        about this household is to be written.

        \param[in] writeHeader If this flag is true then a simple
        comment with the order of the fields is written for future
        reference.
        
        \param[in] delim An optional delimiter between each value
        associated with this node.
    */
    void write(std::ostream& os, const bool writeHeader = false,
               const std::string& delim = " ") const;

    /**
       This is just a convenience string that is used to generate the
       header before the information is written to the model.  This
       value is set by ModelGenerator.
     */
    static std::vector<std::string> pumsHouColNames;
    
private:
    /** A string representing the data for this household to be
        included in the model. The order of the data is the same as
        the sequence of column names as specified by the user using
        model generation.  This value is set when the household is
        create and is never changed.
    */
    std::string houseInfo;

    /** A set of occurrence frequency and strings representing the
        data for each person associated with this household.  The
        order of person information is the same as the sequence of
        column names as specified by the user using model generation.
    */
    PepWtInfoList pepWtInfo;

    /** The number of rooms in the building associated with this
        household.  This information is used to identify the building
        that is suitable for containing this household.  This value is
        the same as the BDSP column from the PUMS household data file.
        This value is set when the household is created and is never
        changed.
    */
    int bedRooms;

    /** This instance variables the BLD column value.  This value is
        set when the household is created and is never changed. */
    int bld;

    /** The PUMA region ID to which this building belongs. This value
        is set when the household is create and is never changed. */
    int pumaID;
    
    /** The number of households of this type to be generated. This
        value is initialized to the WGTP value from the PUMS household
        data.
    */
    int wgtp;

    /** Househod income. -- see
        https://www2.census.gov/programs-surveys/acs/tech_docs/pums/data_dict/PUMS_Data_Dictionary_2017-2021.pdf
        
        Household income (past 12 months, use ADJINC to adjust HINCP to
        constant dollars)
            bbbbbbbb    .N/A (GQ/vacant)
            0           .No household income
            -59999      .Loss of $59,999 or more
            -59998..-1  .Loss of $1 to $59,998
            1..99999999 .Total household income in dollars
                         (Components .are rounded)
    */
    int hincp;

    /** This instance variable tracks the number of people set for
        this household.  The number of people is computed by the
        getInfo method.  This value is set when an actual household is
        created from a reference household in
        PUMS::distributePopulation method.
     */
    int people;

    /**
     * The ID of the building to which this household has been
     * assigned. This value is set only when this household is added
     * to a building.
     */
    int buildingID = -1;

    /**
     * Information about the people in this household.  This value is
     * set when an household is being added to a building.  The
     * information about the people is created in the getInfo method.
     */
    std::vector<PUMSPerson> peopleInfo;
    
    /**
       This vector is used to recode the BLD type when estimating
       building sizes.  This is needed because detached single-family
       homes that have a code of 2 should have higher size than
       attached families.  This vector is used to keep the getSize()
       method in this class fast.  In summary, the recode enables
       better assignment of buildings to households.
    */
    static const std::vector<int> BldRecode;

    /**
       A counter to assign unique person ID values to each person
       generated in the getInfo method.
     */
    static long perIDcounter;
};

#endif
