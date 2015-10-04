#ifndef __haplos__Schedule__
#define __haplos__Schedule__
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
#include <iostream>
#include <vector>

#include "TimeSlot.h"
class TimeSlot;
class Schedule {
    public:
        /** The default constructor for this class
         */
        Schedule();
        
        /** Alternative constructor for this class with specific type
         param[in] type type of schedule to create (0=young child, 1= school aged child, 2=older school aged child, 3=working adult, 4=non-working adult)
         */
        Schedule(int type);
    
        /**Copy Constructor for Schedule
        param[in] s schedule to copy.
         */
        Schedule(const Schedule &s);
    
        void setScheduleType();
    
        /**Return integer representing the schedule type.
         return type of schedule (0=young child, 1= school aged child, 2=older school aged child, 3=working adult, 4=non-working adult)
         */
        int getScheduleType();
    
        /** Set the Location of Job
         param[in] jobLocationID id of job location
         */
         
        void setJobLocation(int jobLocationID);
    
        /**Return the ID of the location of Job
        return ID of Job Location
         */
        int getJobLocation();
    
        /**Create a new timeslot in the Schedule
         param[in] building  pointer to location of where person should be during time slot.
         param[in] endtime end time of time slot.
         */
        void addTimeSlot(TimeSlot t);
    
        /**Removes Time Slot i from Schedule
         param[in] i  number element to remove or -1 to remove the last element
         */
        void removeTimeSlot(int i);
    
        /**Change the schedule type (this will remake schedule).
         param[in] type type to change to (0=young child, 1= school aged child, 2=older school aged child, 3=working adult, 4=non-working adult)
         */
        void setScheduleType(int type);
    
        TimeSlot* getLocationAt(int time);
    
        /**Return current TimeSlot element without advancing the current time
         return pointer to current TimeSlot element in scheduel
         */
        TimeSlot* getCurrentTimeSlot();
    
        /**Advance current time step and Get Location for Next Time Step
            return building ID or transport ID for next timestep
         */
        TimeSlot* getNextLocation();
        
        /**Peek Location for Next Time Step, without advancing currentTimeStep
         return building ID or transport ID for next timestep
         */
        TimeSlot* peekNextLocation();
    
        std::string toString();
        /**
         The destructor.
         
         Currently the destructor does not have any specific task to
         perform in this class.  However, it is defined for adherence
         with conventions and for future extensions.
         */
        virtual ~Schedule();
    
    private:
        int type;
    
        /**Change the schedule type (this will remake schedule).
         param[in] type type of schedule to generate (0=young child, 1= school aged child, 2=older school aged child, 3=working adult, 4=non-working adult)
         */
        static const int MAXTIMESTEPS=1008;   //10 minute intervals
        void generateSchedule(int type);
        std::vector<TimeSlot> plan;
        int currentTimeStep;
        int currentTimeSlot;
        int numberOfTimeSlots;
        int jobLocationID;
};
#endif /* defined(__haplos__Schedule__) */
