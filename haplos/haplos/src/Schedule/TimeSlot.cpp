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
#include "TimeSlot.h"
#include <cstdio>

TimeSlot::TimeSlot(){
    buildingID=-1;
    endTime=0;
}

TimeSlot::TimeSlot(int buildingID, int endTime, char visitorType){
    this->buildingID=buildingID;
    this->endTime=endTime;
    this->visitorType=visitorType;
}

int TimeSlot::getLocation(){
    return buildingID;
}

int TimeSlot::getEndTime(){
    return endTime;
}

int TimeSlot::getDayEndTime(){
    int day = endTime/144;
    return endTime-(day*144);
}

char TimeSlot::getVisitorType(){
    return visitorType;
}

std::string TimeSlot::toString(){
    int day = endTime/144;
    int time = endTime-(day*144);
    int minutes=time*10;
    int hour = minutes/60;
    minutes = minutes%60;
    return "\""+std::to_string(buildingID)+"\",\""+visitorType+"\",\""+std::to_string(endTime)+"\",\""+ std::to_string(day)+"("+std::to_string(hour)+":"+std::to_string(minutes)+")\"";
}

TimeSlot::~TimeSlot(){
    
}