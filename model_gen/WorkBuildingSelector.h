#ifndef WORK_BUILDING_IDENTIFIER_H
#define WORK_BUILDING_IDENTIFIER_H

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
#include "OSMData.h"

/** \brief Estimate travel time between pairs of population rings.

    <p>This class is used by the schedule generator to identify
    potential buildings to where a person could travel for work.
    Specifically, given a work-travel time of \i t minutes, the
    question is which work-building is the best fit?  To answer this
    question, we could generate the travel time from an home-building
    to each work-building.  However, the time taken for checking
    extact paths is way too long (10s of minutes per person).  Of
    course, this does not scale to millions of people. </p>

    <p>The next approach that was tried was to use the orthodromic
    distance as an approximation between buildings.  Using this
    distance we build a list of \i candidate work-buildings and use
    the path to these buildings to find work-locations.  However, this
    approach had the following shortcomings:

    <ol>

    <li>Often 100s of work-buildings would be found and checking this
    list was time consuming.  </li>

    <li>In several cases, a valid building could not be found
    primarily because the the orthodromic distance wasn't a good
    approximation for longer (> 15 minutes) or very short (< 5
    minutes) travel times.<li>

    </ol>

    </p>

    <p>To address the above issue(s), this class provides the
    following alternative approach to determine potential
    work-buildings:

    <ol>

    <li>First, pairwise average distances between each pair of
    population rings is estimated and stored.  This average is
    computed by randomly selecting \i n-pairs of buildings from each
    unique-pair of poulation rings and estimating the travel time by
    generating paths between them.</li>

    <li>Next, these pre-computed pairwise distances are used to
    identify potential buildings in each pair of buildings.</li>

    </ol>

    </p>

    This is class is used to implement the aforementioned approach for
    identifing work-buildings.

    \note This class is designed to be created only once and the
    object instance should be reused. Do not create multiple objects
    of this class.
*/

class WorkBuildingSelector {
public:
    /**
       An internal class used to maintain pairwise (between two rings)
       estimates of travel times.
    */
    class TimeEstimate {
    public:
        /** The mean travel time estimate */
        double mean;

        /** The standard deviation in the travel time estimate */
        double stdev;
    };

public:
    /**
       The only constructor for this class.  
     */
    WorkBuildingSelector(const OSMData& model) : model(model) {}

    /** A destructor.

        This is a dummy destructor and is present merely to adhere to
        coding conventions.
     */
    ~WorkBuildingSelector() {}

    /**
       This is a helper method that must be used to initialize the
       information in this work building selector.  If the
       useTrvlEstFile is specified, then this method loads the data
       from the file.  Otherwise, this method generates the data and
       optionally writes the data to the outTrvlEstFile.

       \param[in] numPairs The number of pairs of buildings to be used
       to generate travel time estimates.

       \param[in] useTrvlEstFile The input file from where the results
       are to be read.  The data must have been written by a prior
       call to this method.  This is usually the value specified for
       the \c --use-trvl-est command-line argument.
       
       \param[in] outTrvlEstFile The output file to where the results
       are to be written.  This is usually the value specified for the
       \c --out-trvl-est command-line argument.
    */
    void genOrUseTrvlEst(const int numBldPairs,
                         const std::string& useTrvlEstFile,
                         const std::string& outTrvlEstFile);

protected:
    /**
       The top-level method that builds an estimate of travel times
       between each pair of population-rings in the supplied model.

       \note This method should be called just once on an object.

       \note This method is multithreaded.

       \param[in] numPairs The number of pairs of buildings to be used
       to generate travel time estimates.
    */
    void generateTravelTimeEstimates(const int numPairs);

    /**
       This method is used to reuse travel estimates that were
       previously written to a data file. This method is used when the
       --use-trvl-est command-line argument is used.

       \param[in] The travel file estimate to be used.  This is
       typically the value specified by the user via the \c
       --use-trvl-est command-line argument.
     */
    void loadTravelTimeEstimates(std::istream& is = std::cin);
    
    /**
       Prints the travel estimates as a grid to ease troubleshooting
       and reporting statistics.

       \param[out] os The output stream to where the grid of values is
       to be printed.
    */
    void printTimeEstimatesMatrix(std::ostream& os = std::cout) const;

    /**
       Internal helper method to populate the list of work-buildings
       for each population ring.  This list is computed when the
       average distances are estimated.  Once the list is created, it
       is used during the lifetime of this object to determine
       potential work buildings.
    */
    void buildPopRingBuildingList();

    /**
       Internal helper method to estimate time between a pair of
       population rings.

       \param[in] ringIdx1 The index of the first ring.

       \param[in] ringIdx2 The index of the second ring.

       \param[in] numPairs The number of pairs of buildings to be used
       to generate travel time estimates.

       \note This method is called from multiple threads. So do not
       use any shared variables/data structures.
    */
    TimeEstimate estimateTime(const size_t ringIdx1, const size_t ringIdx2,
                              const int numPairs) const;

    /**
       Internal helper method to find travel time between a given pair
       of buildings.

       \param[in] bldId1 The ID of the first building.

       \parma[in] bldId2 The ID of the second building.

       \return The travel time computed using a PathFinder object
       between the given pair of buildings in the model.
    */
    double getTime(const long bldId1, const long bldId2) const;
    
private:
    /**
       Reference to the model to be used for identifying work-buildings.
     */
    const OSMData& model;

    /**
       This vector maintains the list of building IDs in a given
       population ring.  This vector is computed once (indirectly from
       the constructor) and is used during the lifetime of this class.

       \see buildPopRingBuildingList
     */
    std::vector<std::vector<long>> popRingWrkBld;

    /**
       The time estimates between a pair of population rings.  The
       time estimates are essentially stored as a matrix of values.
       The same value is stored for ring1,ring2 and for ring2, ring1.  
    */
    std::vector<std::vector<TimeEstimate>> timeEstimates;
};

#endif
