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
    this->jobLocationID=-1;
    

}


Schedule::Schedule(int type){
    this->type=type;
    currentTimeStep=0;
    this->jobLocationID=-1;
}

void Schedule::setJobLocation(int jobLocationID){
    this->jobLocationID=jobLocationID;
}

int Schedule::getJobLocation(){
    return jobLocationID;
}

Schedule::Schedule(const Schedule &s){
    type=s.type;
    currentTimeStep=s.currentTimeStep;
    jobLocationID=s.jobLocationID;
    plan.resize(s.plan.size());
    
    for(int i=0; i< s.plan.size(); i++)
    {
        plan[i] = s.plan[i];
    }

}

void Schedule::addTimeSlot(TimeSlot t){
    plan.push_back(t);
}

void Schedule::removeTimeSlot(int i){
    if(i==-1){
        plan.pop_back();
    }else{
        plan.erase(plan.begin()+i);
    }
    
}

void Schedule::setScheduleType(int type){
    this->type=type;
}

int Schedule::getScheduleType(){
    return type;
}

TimeSlot* Schedule::getLocationAt(int time){
    if( time< plan.size()){
        return &plan[time];
    }
    else{
        return NULL;
    }
}

TimeSlot* Schedule::getCurrentTimeSlot(){
    return &plan[currentTimeSlot];
}

TimeSlot* Schedule::getNextLocation(){
    currentTimeStep++;
    if( currentTimeStep==MAXTIMESTEPS-1 ){
        currentTimeStep=0;
        currentTimeSlot=0;
    }else{
       currentTimeStep++;
    }
    if(plan[currentTimeSlot].getEndTime()>=currentTimeStep){
        //Move to Next Time Slot
        currentTimeSlot++;
    }
    return &plan[currentTimeSlot];
    
}

TimeSlot* Schedule::peekNextLocation(){
    if(plan.size()!=0){
        return &plan[currentTimeSlot];
    }else{
        return NULL;
    }
}

std::string Schedule::toString(){
    std::string wordType ="";
    switch(type){
        case 0:
            wordType="Young Child";
            break;
        case 1:
            wordType = "Young School Child";
            break;
        case 2:
            wordType = "School Child";
            break;
        case 3:
            wordType="Employed Adult";
            break;
        case 4:
            wordType="Unemployed Adult";
            break;
        default:
            wordType="Uknown";
            break;
    }
    std::string returnString= "\"Type:\",\""+wordType+"\"\n";
    returnString+="\"ID\",\"Visitor Type\",\"End Time\",\"Real Time\"\n";
    for(std::vector< TimeSlot >::iterator it = plan.begin(); it!= plan.end(); ++it){
        returnString+=it->toString()+"\n";
    }
    return returnString;
}




Schedule::~Schedule(){
    
}