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

#include "Schedule.h"


Schedule::Schedule() {
    type=0;
    currentTimeStep=0;
}


Schedule::Schedule(int type){
    this->type=type;
    currentTimeStep=0;
}

Schedule::Schedule(Schedule &s){
    type=s.type;
    currentTimeStep=s.currentTimeStep;
    plan = s.plan;
}

void Schedule::addTimeSlot(int locationID, int endTime){
    plan.push_back(TimeSlot(locationID, endTime));
}

void Schedule::setScheduleType(int type){
    this->type=type;
}

int Schedule::getScheduleType(){
    return type;
}

int Schedule::getNextLocation(){
    currentTimeStep++;
    if( currentTimeStep==MAXTIMESTEPS-1 ){
        currentTimeStep=0;
    }else{
       currentTimeStep++;
    }
    if(plan[currentTimeSlot].getEndTime()<currentTimeStep){
        //Stay in Current Time Slot
        return plan[currentTimeSlot].getLocation();
    }else{
        //Move to Next Time Slot
        currentTimeSlot++;
        return plan[currentTimeSlot].getLocation();
        
    }
    
}

int Schedule::peekNextLocation(){
    return plan[currentTimeSlot].getLocation();
}

std::string Schedule::toString(){
    std::string returnString= "\t\t\tType: "+std::to_string(type)+"\n";
    for(std::vector< TimeSlot >::iterator it = plan.begin(); it!= plan.end(); ++it){
        returnString+=it->toString();
    }
    return returnString;
}




Schedule::~Schedule(){
    
}