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

#include "Building.h"

using namespace std;

Building:: Building(){
    this->type = 'U';
    this->idNumber = -1;
    this->location[0] = -1;
    this->location[1] = -1;
    this->maxCapacity = -1;
    this->currentCapacity = 0;
    this->visitorCapacity = 0;

    for(int i = 0; i < 1008; i++){
        this->currentVisitorCapacity[i] = 0;
    }
}

Building::Building(char t, int i, int x, int y, int capacity, int visitorCapacity){
    this->type = t;
    this->idNumber = i;
    this->location[0] = x;
    this->location[1] = y;
    this->maxCapacity = capacity;
    this->currentCapacity = 0;
    this->visitorCapacity = visitorCapacity;

    for(int i = 0; i < 1008; i++){
        this->currentVisitorCapacity[i] = 0;
    }
}

Building::Building(const Building &b){
    //std::cout<<"Copy"<<std::endl;
    //std::cout<<b.currentEmployees.size()<<std::endl;
    this->type = b.type;
    this->idNumber = b.idNumber;
    this->location[0] = b.location[0];
    this->location[1] = b.location[1];
    this->maxCapacity = b.maxCapacity;
    this->currentCapacity = b.currentCapacity;
    this->visitorCapacity = b.visitorCapacity;
    this->currentEmployees = b.currentEmployees;
    this->currentVisitors = b.currentVisitors;
    /*this->currentEmployees.clear();
    this->currentVisitors.clear();
    if(b.currentEmployees.size()>0){
        std::cout<<"Copying Employees"<<std::endl;
        this->currentEmployees.insert(b.currentEmployees.begin(), b.currentEmployees.end());
    }
    if(b.currentVisitors.size()>0){
        this->currentVisitors.insert(b.currentVisitors.begin(), b.currentVisitors.end());
    }*/
}

Building &Building::operator = (const Building &b){
    //std::cout<<"Equal"<<std::endl;
    if (this!=&b) {
        this->type = b.type;
        this->idNumber = b.idNumber;
        this->location[0] = b.location[0];
        this->location[1] = b.location[1];
        this->maxCapacity = b.maxCapacity;
        this->currentCapacity = b.currentCapacity;
        this->visitorCapacity = b.visitorCapacity;
        this->currentEmployees = b.currentEmployees;
        this->currentVisitors = b.currentVisitors;
        
        /*this->currentEmployees.clear();
        this->currentVisitors.clear();
        if(b.currentEmployees.size()>0){
            this->currentEmployees.insert(b.currentEmployees.begin(), b.currentEmployees.end());
        }
        if(b.currentVisitors.size()>0){
            this->currentVisitors.insert(b.currentVisitors.begin(), b.currentVisitors.end());
        }*/
        
    }
    return *this;
}

int Building::getID(){
    return idNumber;
}

int Building::getMaxCapacity(){
    return maxCapacity;
}

int Building::getCurrentCapacity(){
    return currentCapacity;
}

void Building::setCurrentCapacity(int c){
    currentCapacity=c;
}

int Building::getMaxVisitorCapacity(){
    return visitorCapacity;
}

void Building::setMaxVisitorCapacity(int v){
    visitorCapacity = v;
}
int Building::getCurrentVisitorCapacity(int time){
    return currentVisitorCapacity[time];
}

void Building::setCurrentVisitorCapacity(int time, int c){
    currentVisitorCapacity[time]=c;
}

void Building::addVisitorTimeSlot(int startTime, int endTime){
    for(int i = startTime; i<endTime; i++){
        currentVisitorCapacity[i]++;
    }
}

std::unordered_map<int, int> Building::getVisitors(){
    return currentVisitors;
}

std::unordered_map<int, int> Building::getEmployees(){
    return currentEmployees;
}

void Building::addVisitor(int pID, int familyID){
    currentVisitors[pID] = familyID;
}

void Building::removeVisitor(int pID){
    currentVisitors.erase(pID);
}

void Building::addEmployee(int pID, int familyID){
    //std::cout<<"ID: "<<idNumber<<std::endl;
    //std::cout<<"Size: "<<currentEmployees.size()<<std::endl;
    //std::cout<<"Type: "<<type<<std::endl;
    //std::cout<<pID<<" "<<familyID<<std::endl;
    currentEmployees[pID] = familyID;
    attemptsAdded++;
}

void Building::removeEmployee(int pID){
    currentEmployees.erase(pID);
}

int Building::getTotalNumberOfPeople(){
    return currentEmployees.size() + currentVisitors.size();
}

void Building::setID(int id){
    this->idNumber= id;
}
void Building::setLocation(int x, int y){
    this->location[0]=x;
    this->location[1]=y;
}

int* Building::getLocation(){
    //std::cout<<"Location: "<<location[0]<<","<<location[1]<<std::endl;
	return location;
}

char Building::getType() const{
    return type;
}
std::string Building::toString(){
    std::string returnString="ID: "+std::to_string(this->idNumber)+
                             "\n Type: "+std::string(1, this->type)+
                             "\n Capacity: "+std::to_string(this->maxCapacity)+
                             "\n Visitor Capacity: "+std::to_string(this->visitorCapacity)+
                             "\n Location: "+std::to_string(this->location[0])+","+std::to_string(this->location[1]);
    return returnString;
}

std::string Building::exportString(){
    std::string returnString = "*";
    returnString += std::string(1, this->type)+"\n";
    returnString += std::to_string(this->idNumber)+"\n";
    returnString += std::to_string(this->maxCapacity)+"\n";
    returnString += std::to_string(this->visitorCapacity)+"\n";
    returnString += std::to_string(this->location[0])+","+std::to_string(this->location[1])+"\n";
    return returnString;
}

Building::~Building(){
    
    
}

