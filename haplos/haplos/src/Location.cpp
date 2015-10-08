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

#include "Location.h"
#include <cstdlib>
#include <iostream>



using namespace std;

Location::Location(){
	numberOfPeople=0;
	maxPopulation=0;
    density=0;
	coordinates[0]=-1;
	coordinates[1]=-1;

}

Location::Location(int x, int y, float m, float d, int hubID) {
	// TODO Auto-generated constructor stub
	if(m>=0){
        numberOfPeople=0;
    }else{
        numberOfPeople=-1;
    }
	maxPopulation=m;
    density=d;
	coordinates[0]=x;
	coordinates[1]=y;
    tHub = TransportHub(hubID, x, y);
}

int Location::getCurrentPopulation() {
    int total = 0;
    for ( auto it = buildings.begin(); it != buildings.end(); ++it ){
        switch(it->second->getType()){
            case 'B':
                //Business
                total += it->second->getTotalNumberOfPeople();
                break;
            case 'M':
                //Medical
                total += (static_cast<Medical *> (it->second))->getTotalNumberOfPeople();
                break;
            case 'S':
                //School
                total += (static_cast<School *> (it->second))->getTotalNumberOfPeople();
                break;
            case 'D':
                //Daycare
                total += (static_cast<Daycare *> (it->second))->getTotalNumberOfPeople();
                break;
            default:
                //Standard Building without Specific Type
                total += it->second->getTotalNumberOfPeople();
                break;
        }
        
    }
    
    
    
	return total+tHub.getTotalNumberOfPeople();
}

std::unordered_map<int,Person*>* Location::getPeople(){
    return &people;
}

float Location::getDensity() const{
    return density;
}

bool Location::isFull(){
	if (numberOfPeople>=(int)maxPopulation) {
		return true;
	}
	else{
		return false;
	}
}
float Location::getMaxPopulation() const{
	return maxPopulation;
}

int* Location::getCoordinates(){
	return coordinates;
}

int Location::getNumberOfBuildings(char type) const{

    if(type=='\0'){
        //Get Count for All Buildings
        return buildings.size();
    }else{
        //Get Count for Specific Type of Building
        int count =0;
        int i=0;
        for ( auto it = buildings.begin(); it != buildings.end(); ++it ){
            if(it->second->getType()==type){
                count++;
            }
            if(it->second->getType()!='M'&&it->second->getType()!='B'&&it->second->getType()!='H'&&it->second->getType()!='S'&&it->second->getType()!='D'){
                std::cout<<"------------"<<std::endl;
                std::cout<<"Location: "<<coordinates[0]<<", "<<coordinates[1]<<std::endl;
                std::cout<<it->second->toString()<<std::endl;
                std::cout<<i<<"/"<<buildings.size()<<std::endl;
                std::cout<<"------------"<<std::endl;

            }
            i++;
        }
        return count;
    }
}

void Location::addFamily(Family *f){
    Person* people =f->getAllPersons();
    int numberOfPeople = f->getNumberOfPeople();
    for(int p=0; p<numberOfPeople;p++){
        this->addPerson(&people[p]);
    }
}

Building* Location::addBuilding(Building *b){
    
    buildings.insert({b->getID(), b});
    return buildings[b->getID()];
}

void Location::addPerson(Person *p){
    people[p->getID()]= p;
    numberOfPeople++;
    
}

void Location::removePerson(int idNum){
    people.erase(idNum);
	numberOfPeople--;
}

TransportHub* Location::getTransportHub(){
        return &tHub;
}

Building* Location::hasAvaliableBuilding(char visitorType, int startTime, int endTime, int numberOfVisitors){
    for ( auto it = buildings.begin(); it != buildings.end(); ++it ){
        switch(visitorType){
            case 'E':
                if(it->second->getType()=='B' || it->second->getType()=='M' || it->second->getType()=='S'){
                   //std::cout<<"Building Found that can have Employee"<<std::endl;
                    //std::cout<<"Max Capacity: "<<it->second->getMaxCapacity()-it->second->getCurrentCapacity()<<std::endl;
                    //std::cout<<"Current Capacity: "<<it->second->getCurrentCapacity()<<std::endl;
                    if(it->second->getMaxCapacity()-it->second->getCurrentCapacity()>numberOfVisitors){
                        //std::cout<<"FOUND Employeer"<<std::endl;
                        return it->second;
                    }
                }
                break;
            case 'V':
            {
                int invalid=false;
                if(it->second->getMaxVisitorCapacity()> 0){
                    for(int i = startTime; i<endTime; i++){
                        if(it->second->getMaxVisitorCapacity()-it->second->getCurrentVisitorCapacity(i)<numberOfVisitors){
                            invalid=true;
                            break;
                        }
                    }
                   // std::cout<<"FOUND Visitor"<<std::endl;
                    if(!invalid){
                        //std::cout<<"\t\t Max Visitors At ("<<it->second->getID()<<") "<<it->second->getMaxVisitorCapacity()<<std::endl;
                        return it->second;
                    }
                }
                break;
            }
            case 'P':
                if(it->second->getType()=='M'){
                    Medical* b = static_cast<Medical* >(it->second);
                    if(b->getMaxPatientCapacity()-b->getCurrentPatientCapacity()>=numberOfVisitors){
                      //  std::cout<<"FOUND Patient"<<std::endl;
                        return it->second;
                    }
                }
                break;
            case 'S':
                //Student
                if(it->second->getType()=='S'){
                    School* b = static_cast<School* >(it->second);
                    if(b->hasGradeAvaliable(startTime, numberOfVisitors)){
                        //std::cout<<"FOUND Student"<<std::endl;
                        return it->second;
                    }

                }
                break;
            case 'D':
                //Daycare
                if(it->second->getType()=='D'){
                    Daycare* b = static_cast<Daycare* >(it->second);
                    if(b->getMaxChildCapacity()-b->getChildCapacity()>=numberOfVisitors){
                        return it->second;
                    }
                    
                }
                break;
                
        }
    }
    return NULL;
}


void Location::printTemp(){
    std::cout<<"Print Temp"<<std::endl;
    std::cout<<tmp<<std::endl;
    Medical *m = static_cast<Medical *>(tmp);
    std::cout<<m<<std::endl;
    std::cout<<m->toString()<<std::endl;
    m->setCurrentPatientCapacity(m->getCurrentPatientCapacity()+20);
    std::cout<<m->toString()<<std::endl;

}

Location &Location::operator = (const Location &p){
	if (this!=&p) {
		numberOfPeople=p.numberOfPeople;
		maxPopulation=p.maxPopulation;
		coordinates[0]=p.coordinates[0];
		coordinates[1]=p.coordinates[1];
        density=p.density;
	}
	return *this;
}
Location::~Location() {
	// TODO Auto-generated destructor stub

}

