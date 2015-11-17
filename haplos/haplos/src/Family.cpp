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
    childCareAdultPos=-1;
    daycare=NULL;
    this->members.clear();

}

Family::Family(Building *h, Daycare *d, int familyID){
    hasAdult=false;
    hasYoungChild=0;
    hasYoungSchoolChild=false;
    hasSchoolChild=false;
    numberOfPeople=0;
    homeNumber=h;
    childCareAdultPos=-1;
    daycare=d;
    this->familyID = familyID;
    this->members.clear();

}

Family::Family(const Family &f){
    this->hasAdult = f.hasAdult;
    this->hasYoungChild = f.hasYoungChild;
    this->hasYoungSchoolChild = f.hasYoungSchoolChild;
    this->hasSchoolChild = f.hasSchoolChild;
    this->numberOfPeople = f.numberOfPeople;
    this->homeNumber = f.homeNumber;
    this->childCareAdultPos = f.childCareAdultPos;
    this->daycare = f.daycare;
    this->members.clear();
    this->members.insert(f.members.begin(), f.members.end());
    
    this->familyID = f.familyID;
    this->numberOfPeople = f.numberOfPeople;
}

Person* Family::getPerson(int id){
    return &members[id];
    
}

int Family::getNumberOfPeople(){
    return numberOfPeople;
    
}

std::unordered_map< int , Person> * Family::getAllPersons(){
    return &members;
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

int Family::getID(){
    return familyID;
}

void Family::setID(int f){
    this->familyID = f;
    
}

void Family::addPerson(Person newPerson){
    members.insert({newPerson.getID(), Person(newPerson)} );
    if(newPerson.getAge()>17){
        hasAdult=true;
        if(childCareAdultPos == -1){
            childCareAdultPos=newPerson.getID();
        }else{
            if(newPerson.getSchedule()->getScheduleType()==4 &&
               members[childCareAdultPos].getSchedule()->getScheduleType()!=4){
                //Non-Working Adult is being added
                childCareAdultPos=newPerson.getID();
            }
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
    for(auto it = members.begin(); it!= members.end(); ++it){
        it->second.setLocation(x);
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
    return this->getPerson(childCareAdultPos);
    
}

std::string Family::toString(){
    std::ostringstream outputString;
    outputString << "\"Family Home Number:\",\""<< homeNumber->getID() << '"'<< std::endl;
    outputString << "\"Number of People: \",\"" << numberOfPeople <<'"'<< std::endl;
    outputString << "\"Child Care Adult: \",\"" << std::to_string(members[childCareAdultPos].getID())+" (" <<std::to_string(members[childCareAdultPos].getSchedule()->getScheduleType())<<")"<< '"'<< std::endl;
    outputString << "\"Has School Child: \",\"" << ((hasSchoolChild) ? "Yes" : "No") << '"'<< std::endl;
    outputString << "\"Has Young School Child: \",\"" << ((hasYoungSchoolChild) ? "Yes" : "No") << '"'<< std::endl;
    outputString << "\"Has Young Child: \",\"" << ((hasYoungChild) ? "Yes" : "No") << '"'<< std::endl;
    outputString << "\"Detail Information: \"\n" << std::endl;
    
    for(auto it = members.begin(); it!= members.end(); ++it){
        outputString << it->second.toString();
    }
    return outputString.str();
}

std::string Family::exportFamily(){
    std::ostringstream outputString;
    outputString << "%";
    outputString << homeNumber->getID() << std::endl;
    int daycareID = -1;
    if(daycare != NULL){
        daycareID = daycare->getID();
    }
    outputString << daycareID << std::endl;
    for(auto it = members.begin(); it!= members.end(); ++it){
       outputString << it->second.exportPerson();
    }
    return outputString.str();
    
}

void Family::updateToNextTimeStep(std::unordered_map<int, Building*> *allBuildings){
    //std::cout<<"Family: "<<homeNumber->getID()<<std::endl;
    //std::cout<<this->toString()<<std::endl;
    for(auto m = members.begin(); m != members.end(); m++){
        //Person p = m->second;
        TimeSlot* oldLocation = m->second.getCurrentTimeSlot();
        int buildingID = oldLocation->getLocation();

        //Update Previous Location
        switch(oldLocation->getVisitorType()){
            case 'H':
                //Home
                allBuildings->at(buildingID)->removeVisitor(m->second.getID());
                break;
            case 'S':
                //Student
                static_cast<School *> (allBuildings->at(buildingID))->removeStudent(m->second.getID());
                break;
            case 'D':
                //Child
                static_cast<Daycare *> (allBuildings->at(buildingID))->removeChild(m->second.getID());
                break;
            case 'V':
                //Visitor
                allBuildings->at(buildingID)->removeVisitor(m->second.getID());
                break;
            case 'E':
                //Employee
                allBuildings->at(buildingID)->removeEmployee(m->second.getID());
                break;
            case 'P':
                //Patient
                static_cast<Medical *> (allBuildings->at(buildingID))->removePatient(m->second.getID());
                break;
            case 'T':
                static_cast<TransportHub *> (allBuildings->at(buildingID))->removeVisitor(m->second.getID());
                break;
            case 'W':
                static_cast<TransportHub *> (allBuildings->at(buildingID))->removeEmployee(m->second.getID());
                break;
            case 'C':
                static_cast<TransportHub *> (allBuildings->at(buildingID))->removePrivateTransport(m->second.getID(), familyID);
                break;
            default:
                std::cout<< "Invalid Visitor Type in Old: "<<oldLocation->getVisitorType()<<std::endl;
                std::cout<<"ID: "<<m->second.getID()<<std::endl;
                std::cout<<m->second.getSchedule()->toString();
                break;
        }
        
        TimeSlot* newLocation=m->second.updateToNextTimeStep();
        buildingID = newLocation->getLocation();
        Building *tmp = allBuildings->at(buildingID);
        //std::cout<<"All Building Size: "<<allBuildings->size()<<std::endl;
        //std::cout<<"Building  ID: "<<tmp->getID()<<std::endl;
        //std::cout<<"Building  Type: "<<tmp->getType()<<std::endl;
        //Update to New Location
        switch(newLocation->getVisitorType()){
            case 'H':
                //Home
                allBuildings->at(buildingID)->addVisitor(m->second.getID(), familyID);
                break;
            case 'S':
                //Student
                static_cast<School* > (allBuildings->at(buildingID))->addStudent(m->second.getID(), familyID);
                break;
            case 'D':
                //Child
                static_cast<Daycare* > (allBuildings->at(buildingID))->addChild(m->second.getID(), familyID);
                break;
            case 'V':
                //Visitor
                allBuildings->at(buildingID)->addVisitor(m->second.getID(), familyID);
                break;
            case 'E':
                //Employee
                //std::cout<<"Building ID: "<<buildingID<<std::endl;
                allBuildings->at(buildingID)->addEmployee(m->second.getID(), familyID);
                break;
            case 'P':
                //Patient
                static_cast<Medical* > (allBuildings->at(buildingID))->addPatient(m->second.getID(), familyID);
                break;
            case 'T':
                static_cast<TransportHub *> (allBuildings->at(buildingID))->addVisitor(m->second.getID(), familyID);
                break;
            case 'W':
                static_cast<TransportHub *> (allBuildings->at(buildingID))->addEmployee(m->second.getID(), familyID);
                break;
            case 'C':
                static_cast<TransportHub *> (allBuildings->at(buildingID))->addPrivateTransport(m->second.getID(), familyID);
                break;
            default:
                std::cout<< "Invalid Visitor Type in New: "<<oldLocation->getVisitorType()<<std::endl;
                std::cout<<"ID: "<<m->second.getID()<<std::endl;
                std::cout<<m->second.getSchedule()->toString();
                break;

        }
        
    }
    
}

Family::~Family() {
	// TODO Auto-generated destructor stub
}
