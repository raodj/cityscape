

#ifndef __haplos__TimeSlot__
#define __haplos__TimeSlot__
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
class TimeSlot{
    public:
        /** The default constructor for this class
         */
        TimeSlot();
    
        /** Alternative constructor for this class with specific type
         param[in] building Pointer to location person will be at during time slot.
         param[in] endTime time where person will move to next location.
         */
        TimeSlot(int buildingID, int endTime, char visitorType);
    
        /** Get End Time for given Time Slot
            return end time.
         */
        int getEndTime();
    
        /** Get Location ID where person should be in given time slot.
            return Locaiton ID of place person should be.
         */
        int getLocation();
    
        std::string toString();
    
        /**
         The destructor.
         
         Currently the destructor does not have any specific task to
         perform in this class.  However, it is defined for adherence
         with conventions and for future extensions.
         */
        virtual ~TimeSlot();
    private:
        int buildingID;
        int endTime;
        char visitorType;
};

#endif /* defined(__haplos__TimeSlot__) */
