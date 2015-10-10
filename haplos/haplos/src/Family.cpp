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
#include "Buildings/Medical.h"
#include "Buildings/TransportHub.h"
#include <vector>
#include <sstream>

using namespace std;

Family::Family() {
    hasAdult=false;
    hasYoungChild=0;
    hasYoungSchoolChild=false;
    hasSchoolChild=false;
    numberOfPeople=0;
    homeNumber=NULL;
    childCareAdultPos=0;
    daycare=NULL;
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

void Family::setDaycare(Daycare *d){
    daycare= d;
}

Building* Family::getHome(){
    return homeNumber;
}

Daycare* Family::getDaycare(){
    return daycare;
}

void Family::addPerson(Person newPerson){
    members.push_back(newPerson);
    if(newPerson.getAge()>17){
        hasAdult=true;
        if(newPerson.getSchedule()->getScheduleType()==4 &&
           members.at(childCareAdultPos).getSchedule()->getScheduleType()!=4){
            //Non-Working Adult is being added
            childCareAdultPos=members.size()-1;
        }
    }else{
        if(newPerson.getAge()<14){
            //Has Child that Needs to be Supervised by Parent
            if(newPerson.getAge()>4){
                hasYoungSchoolChild=true;
            }else{
                hasYoungChild++;
            }
        }else{
            hasSchoolChild=true;
        }
    }
    numberOfPeople++;
    
}

void Family::setLocation(int x){
    for(std::vector< Person >::iterator it = members.begin(); it!= members.end(); ++it){
        it->setLocation(x);
    }
}

bool Family::getHasAdult(){
    return hasAdult;
}

bool Family::getHasSchoolChild(){
    return hasSchoolChild;
}

bool Family::getHasYoungSchoolChild(){
    return hasYoungSchoolChild;
}

int Family::getHasYoungChild(){
    return hasYoungChild;
}

Person* Family::getChildCareAdult(){
    return &members.at(childCareAdultPos);
    
}


std::string Family::toString(){
    std::ostringstream outputString;
    outputString << "\"Family Home Number:\",\""<< homeNumber->getID() << '"'<< std::endl;
    outputString << "\"Number of People: \",\"" << numberOfPeople <<'"'<< std::endl;
    outputString << "\"Child Care Adult: \",\"" << std::to_string(members.at(childCareAdultPos).getID())+" (" <<std::to_string(members.at(childCareAdultPos).getSchedule()->getScheduleType())<<")"<< '"'<< std::endl;
    outputString << "\"Has School Child: \",\"" << ((hasSchoolChild) ? "Yes" : "No") << '"'<< std::endl;
    outputString << "\"Has Young School Child: \",\"" << ((hasYoungSchoolChild) ? "Yes" : "No") << '"'<< std::endl;
    outputString << "\"Has Young Child: \",\"" << ((hasYoungChild) ? "Yes" : "No") << '"'<< std::endl;
    outputString << "\"Detail Information: \"\n" << std::endl;
    for(std::vector< Person >::iterator it = members.begin(); it!= members.end(); ++it){
        outputString << it->toString();
    }
    return outputString.str();
}

void Family::updateToNextTimeStep(std::unordered_map<int, Building*> *allBuildings){
   // std::cout<<"Family: "<<homeNumber->getID()<<std::endl;
    for (std::vector<Person>::iterator i = members.begin(); i!= members.end(); i++){
        //std::cout<<"\t"<<i->getID()<<std::endl;
        TimeSlot* oldLocation = i->getCurrentTimeSlot();
        int buildingID = oldLocation->getLocation();
        //Update Previous Location
        switch(oldLocation->getVisitorType()){
            case 'H':
                //Home
                allBuildings->at(buildingID)->removeVisitor(&(*i));
                break;
            case 'S':
                //Student
                static_cast<School *> (allBuildings->at(buildingID))->removeStudent(&(*i));
                break;
            case 'D':
                //Child
                static_cast<Daycare *> (allBuildings->at(buildingID))->removeChild(&(*i));
                break;
            case 'V':
                //Visitor
                allBuildings->at(buildingID)->removeVisitor(&(*i));
                break;
            case 'E':
                //Employee
                allBuildings->at(buildingID)->removeEmployee(&(*i));
                break;
            case 'P':
                //Patient
                static_cast<Medical *> (allBuildings->at(buildingID))->removePatient(&(*i));
                break;
            case 'T':
                static_cast<TransportHub *> (allBuildings->at(buildingID))->removeVisitor(&(*i));
                break;
            case 'W':
                static_cast<TransportHub *> (allBuildings->at(buildingID))->removeEmployee(&(*i));
                break;
            case 'C':
                static_cast<TransportHub *> (allBuildings->at(buildingID))->removePrivateTransport(homeNumber->getID(), &(*i));
                break;
            default:
                std::cout<< "Invalid Visitor Type in Old: "<<oldLocation->getVisitorType()<<std::endl;
                break;
        }
        
        TimeSlot* newLocation=i->updateToNextTimeStep();
        buildingID = newLocation->getLocation();
        //Update to New Location
        switch(newLocation->getVisitorType()){
            case 'H':
                //Home
                allBuildings->at(buildingID)->addVisitor(&(*i));
                break;
            case 'S':
                //Student
                static_cast<School* > (allBuildings->at(buildingID))->addStudent(&(*i));
                break;
            case 'D':
                //Child
                static_cast<Daycare* > (allBuildings->at(buildingID))->addChild(&(*i));
                break;
            case 'V':
                //Visitor
                allBuildings->at(buildingID)->addVisitor(&(*i));
                break;
            case 'E':
                //Employee
                allBuildings->at(buildingID)->addEmployee(&(*i));
                break;
            case 'P':
                //Patient
                static_cast<Medical* > (allBuildings->at(buildingID))->addPatient(&(*i));
                break;
            case 'T':
                static_cast<TransportHub *> (allBuildings->at(buildingID))->addVisitor(&(*i));
                break;
            case 'W':
                static_cast<TransportHub *> (allBuildings->at(buildingID))->addEmployee(&(*i));
                break;
            case 'C':
                static_cast<TransportHub *> (allBuildings->at(buildingID))->addPrivateTransport(homeNumber->getID(), &(*i));
                break;
            default:
                std::cout<< "Invalid Visitor Type in New: "<<oldLocation->getVisitorType()<<std::endl;
                break;

        }
        //Update Building Information
        
        
    }
    
}
Family::~Family() {
	// TODO Auto-generated destructor stub
}
