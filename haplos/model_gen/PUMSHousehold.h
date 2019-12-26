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

/** Shortcut to refer to PWGTP and user info for each household */
using PepWtInfo = std::pair<int, std::string>;

/** A list of occurrence and user information values for each household */
using PepWtInfoList = std::vector<PepWtInfo>;

/** Class to encapsulate necessary information about a household and
    people living in them.

    This class is used to encapsulate information about each household
    that is used to pulate the buildings in the generated model.  Each
    household is identified by a unique SERIALNO set for them in the
    PUMS files.
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

        \param[in] pwgtp The weight for this PUMA record that
        indicates the number of occurrences of this type of household.
    */
    PUMSHousehold(const std::string houseInfo, int bedRooms, int bldCode,
                  int pumaID, int pwgtp);

    /** The destructor.

        Has nothing much to do because this class currently does not
        use any dynamic memory.
     */
    ~PUMSHousehold() {}

    /** Helper method to add a person to this household.

        \param[in] occurrence The number of occurrences of this
        specific person record.
        
        \param[in] info The information about the person to be added.
        This information is in the form of a CSV string. The columns
        are in the order specified by the user.
    */
    void addPersonInfo(const int occurrence, const std::string& info);

    /** Get a size estimate for this household based on apartment
        configuration and number of bedrooms.  This method returns a
        value in the range 0 to 1099 based on the apartment code and
        number of bedrooms.  The return value is useful for sorting
        the households based on the estimated building size to which
        they are to be assigned.
    */
    size_t getSize() const { return bld * 100 + bedRooms; }

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
    */       
    int getAptInBld() const;
    
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
    
    /** The number of households of this type still remaining. This
        value is initialized to the WGTP value from the PUMS household
        data. Each time a household of this type is created, this
        value is decremented by 1 to track the remaining households to
        be created.
    */
    int remaining;
};

#endif
