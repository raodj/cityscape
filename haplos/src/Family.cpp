//------------------------------------------------------------
//
// This file is part of HAPLOS availabe off the website at
// <http://pc2lab.cec.miamiOH.edu/haplos>
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

#include "Family.h"
#include <vector>
#include <sstream>

using namespace std;

Family::Family() {
    hasAdult=false;
    hasYoungChild=false;
    hasChild=false;
    numberOfPeople=0;
    homeNumber=NULL;
    nonWorkingAdultPos=-1;
}

Person* Family::getPerson(int id){
    return &members[id];
    
}

int Family::getNumberOfPeople(){
    return numberOfPeople;
    
}

Person* Family::getAllPersons(){
    return &members[0];
}

void Family::setHome(Building *n){
    homeNumber= n;

}

Building* Family::getHome(){
    return homeNumber;
}

void Family::addPerson(Person newPerson){
    members.push_back(newPerson);
    if(newPerson.getAge()>17){
        hasAdult=true;
        if(newPerson.getSchedule()->getScheduleType()==4 && nonWorkingAdultPos==-1){
            //Non-Working Adult
            nonWorkingAdultPos=members.size()-1;
        }
    }else{
        hasChild=true;
        if(newPerson.getAge()<14){
            //Has Child that Needs to be Supervised by Parent
            hasYoungChild=true;
        }
    }
    numberOfPeople++;
    
}

void Family::setLocation(int x, int y){
    for(std::vector< Person >::iterator it = members.begin(); it!= members.end(); ++it){
        it->setLocation(x, y);
    }
}

bool Family::getHasAdult(){
    return hasAdult;
}

bool Family::getHasChild(){
    return hasChild;
}

bool Family::getHasYoungChild(){
    return hasYoungChild;
}

Person* Family::getNonWorkingAdult(){
    if(nonWorkingAdultPos==-1){
        return NULL;
    }else{
        return &members.at(nonWorkingAdultPos);
    }
}

std::string Family::toString(){
    std::ostringstream outputString;
    outputString << "\"Family Home Number:\",\""<< homeNumber->getID() << '"'<< std::endl;
    outputString << "\"Number of People: \",\"" << numberOfPeople <<'"'<< std::endl;
    outputString << "\"Has Non-Working Adult: \",\"" << ( (nonWorkingAdultPos!=-1) ? "Yes("+std::to_string(members.at(nonWorkingAdultPos).getID())+")" : "No" ) << '"'<< std::endl;
    outputString << "\"Has Child: \",\"" << ((hasChild) ? "Yes" : "No") << '"'<< std::endl;
    outputString << "\"Has Young Child: \",\"" << ((hasYoungChild) ? "Yes" : "No") << '"'<< std::endl;
    outputString << "\"Detail Information: \"" << std::endl;
    for(std::vector< Person >::iterator it = members.begin(); it!= members.end(); ++it){
        outputString << it->toString();
    }
    return outputString.str();
}
Family::~Family() {
	// TODO Auto-generated destructor stub
}
