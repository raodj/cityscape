#ifndef PUMS_PERSON_H
#define PUMS_PERSON_H

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

#include <vector>
#include <string>

/**
 * This class encapsulates information about an individual in the PUMS
 * data. Earlier (i.e., prior to Jun 19 2024), the person information
 * was just stored as a flat CSV entry in the household.  However,
 * this format was not conducive to generate and store weekly activity
 * information for a given person. Hence, the person information has
 * been broken out into a separate class to ease managing schedule
 * data for each person.
 *
 * \note This class is used in the following 2 different contexts:
 *
 *    1. To store raw read from the PUMS_H file (in this case
 *       perID is -1)
 *
 *    2. Store information about each person assigned to an household
 *       (in this case perID isn't -1)
 */
class PUMSPerson {
public:
    /** A default constructor to ease adding person entries to
        standard containers.  The constructor merely initializes the
        values to invalid settings.
    */
    PUMSPerson() {}
    
    /** Constructor method used in PUMS::loadPeopleInfo to create an
        entry for a person record using information in specific
        columns.

        \param[in] colIndexs The index of the columns whose values are
        to be combined into a CSV

        \param[in] info The information to be used to build the CSV

        \return Returns a string in CSV format with the information
        for specific column indexs.
    */    
    PUMSPerson(const std::string& householdID,
               const std::vector<int>& colIndexs,
               const std::vector<std::string>& info);

    /**
     * Returns the information associated with this person object as a
     * csv string.
     *
     * \return A comma separated string of values associated with this
     * person.
     */
    std::string getInfo() const;

    /**
     * Get unique ID assigned for this person.
     *
     * \return THe unique ID associated wih this person ID.
     */
    long getPerID() const { return perID; }

    /**
     * Set a unique ID for this person record as it is going to be
     * added to an household.
     *
     * \param[in] id The unique ID to be assigned to this person record.
     */
    void setPerID(const long id) { perID = id; }
    
    /** Load the data for this person from a given input stream.

        This is a helper method that is used to read information for
        this person from a given input stream (when loading an already
        generated model).  This method assumes that the data has been
        written by a prior call to the write method in this class.

        \param[in,out] is The input stream with 1 line of data from
        where the information for this household is to be read.
    */
    void read(std::istream& is);

    /** Helper method to load column titles from a given string.

        \param[in] titleCommentLine The comment line from where the
        titles for information for a person record is to be read. 
     */
    static void readColTitles(const std::string& titleCommentLine);
    
    /** Write this person information to a given output stream.

        This is a helper method that is used to write the information
        associated with this person to a given output stream.

        \param[out] os The output stream to where the information
        about this household is to be written.

        \param[in] writeHeader If this flag is true then a simple
        comment with the order of the fields is written for future
        reference.
        
        \param[in] delim An optional delimiter between each value
        written by this method.
    */
    void write(std::ostream& os, const bool writeHeader = false,
               const std::string& delim = " ") const;

    /**
     * Set the column names (or column titles) for the information
     * that is going to be stored in each person record.  This method
     * is invoked once by PUMS::loadPeopleInfo()
     *
     * \param[in] colTitles The full list of column titles from the pums_h
     * file for which information is being stored.
     */
    static void setColumnTitles(const std::vector<std::string>& colTitles,
                                const std::vector<int>& colIndexs);

    int getIntegerInfo(const int infoIdx) const;

    /** This is a helper method that generates a schedule to associate
        a work building with this person. The schedule is generated
        for 5 days a week with this person going to work at 8 am and
        returning back home at 5 pm.

        \param[in] homeBldId The ID of the home building.
        
        \param[in] bldId The ID of the work building for this person.

        \param[in] leave2workTime The time when this person is
        scheduled to leave from the home building to the work
        building. This time is in seconds after midnight. The default
        is is 8 am (8 * 3600 = 28800).

        \param[in] leaveFromworkTime The time when this person is
        scheduled to leave from the work building and return back to
        the home building. This time is in seconds after midnight. The
        default is is 5 pm (17 * 3600 = 61200).
    */
    void setWorkBuilding(const long homeBldId, const long bldId,
                         const long leave2workTime = 28800,
                         const long leaveFromWorkTime = 61200);
    
private:
    /**
     * This variable is used to track a unique ID value assigned for
     * each person record associated with a given household. Until a
     * person is assigned to an household this value will be -1.
     */
    long perID = -1;

    /**
     * The household ID associated with this person.  This information
     * is set when the person record is created.
     */
    std::string householdID;
    
    /**
     * Any daily/weekly schedule associated with this person.
     */
    std::string schedule;

    /**
     * This list of values just maintains the list of information
     * associated with this person. The order of information is in the
     * same order as the colTitles list.
     */
    std::vector<int> info;
    
    /**
     * The names of the columns for which the data is being stored in
     * the info list in each person object.
     *
     * \note This instance variable is intentionally static so that we
     * don't need to store the same set of column titles for each
     * object (as it saves a lot of memory).
     */
    static std::vector<std::string> colTitles;
};

#endif
